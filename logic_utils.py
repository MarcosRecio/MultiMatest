import itertools
import pandas as pd

from auxiliar_utils import parallel_threads
from functools import partial

def unary_operator(matrix):
    return lambda x: matrix[x]

def binary_operator(matrix):
    return lambda x, y: matrix[x][y]

def evaluate_interpretation(interpretation, inputs, values):
    # The evaluation takes the values from the interpretation
    evaluation = interpretation.copy()

    # Iterates over each of the items of the interpretation
    for index, symbol in enumerate(evaluation):
        # If the symbol is a function, not a value, interpret the expression
        if not symbol in values:
            # The places where the function is applied in the interpretation are always of lower index (the parser orders the expression)
            places = list(map(lambda x: index+x, inputs[index]))
            
            # Picks the values of the places in order to feed the function
            vals = list(map(lambda x: evaluation[x], places))

            # Get the values of the function and replace the item for the value in the evaluation
            val = symbol(*vals)
            evaluation[index] = val

    return evaluation


def evaluate_formula_in_system(formula, system, logic, verbose = False):

    # Parse the formula
    formula_parsed = logic.parse(formula)
    inputs = formula_parsed.inputs
    formula_to_interpret = list(map(lambda x: x.decode('ascii'),formula_parsed.ops.copy()))

    # Take the logic functions for the system, the values and the minimum designated value
    logic_functions = system['matrix']
    values = system['values']
    min_designated = system['min_designated']

    # Calculate the designated values taking into account the minimum designated value
    designated_values = list(filter(lambda x: x>=min_designated, values))

    # Take the predicates to be substituted for values
    formula_predicates = list(sorted(set(filter(lambda x: x in logic.language.predicates, formula_to_interpret))))

    # Calculate all the possible interpretations and create the replacements for the predicates
    interpretations = list(itertools.product(values, repeat = len(formula_predicates)))
    replacements = [dict(zip(formula_predicates, interpretation)) for interpretation in interpretations]

    # Replace the logic symbols for their logic functions related to the matrices
    interpret_function = list(map(logic_functions.get, formula_to_interpret, formula_to_interpret))
    formula_interpretations = [list(map(replacement.get, interpret_function, interpret_function)) for replacement in replacements]

    # Evaluate the formula
    evaluations = list(map(lambda interpretation: evaluate_interpretation(interpretation, inputs, values), formula_interpretations))

    # Create DataFrame with the results
    evaluation_results = pd.DataFrame(evaluations, columns=formula_to_interpret,index=interpretations)
    evaluation_results.index.name = str(formula_predicates)

    
    if verbose:
        print(formula)
        print(evaluation_results)
        # Check if all the values of the last evaluation (last column) are in the designated values
        result_evaluation = list(map(lambda x: x in designated_values, evaluation_results[evaluation_results.columns[-1]].values))
        is_tautology = all(result_evaluation)

        # Select the index of the places where the formula does not hold a designated value
        fail_index = list(set([x if not y else None for x,y in enumerate(result_evaluation)]) - set([None]))
        fails_results = evaluation_results.loc[[interpretations[index] for index in fail_index]]
        # Print if is tautological or show the places where the formula is not verified by the matrix
        if is_tautology:
            print('Tautology')
        else: 
            print(fails_results)
        
        return evaluation_results, is_tautology, fails_results
        
    else:
        
        return evaluation_results
    


def evaluate_formulas_in_system(system, formulas, logic):
    evaluate_in_system = partial(evaluate_formula_in_system, system = system, logic= logic)
    evaluations = parallel_threads(function=evaluate_in_system, iterable=formulas, threads=len(formulas))
    return dict(zip(formulas, evaluations))

def evaluate_formulas_in_systems(systems, formulas, logic):
    evaluate_in_system =  partial(evaluate_formulas_in_system, formulas=formulas, logic= logic)
    iterable = list(systems.values())
    system_evaluations = parallel_threads(function = evaluate_in_system, iterable = iterable, threads = len(iterable))
    return dict(zip(systems.keys(),system_evaluations))

def formula_evaluation_to_text(formula, evaluation, sep = '-'*20):
    return f"\n{formula}\n{evaluation.to_string()}\n{sep}"

def system_evaluations_to_text(evaluation_dict: dict, sep = '-'*20):
    lines = [formula_evaluation_to_text(formula=formula, evaluation=evaluation, sep = sep) for formula, evaluation in evaluation_dict.items()]
    return ''.join(lines)

def general_evaluation_to_text(evaluation_dict, sep = '#'*20):
    lines = [f'\n{system_name}\n{system_evaluations_to_text(system_evaluation_dict)}\n{sep}'
            for system_name, system_evaluation_dict in evaluation_dict.items()]
    return ''.join(lines)