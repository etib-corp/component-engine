from typing import Any, Callable, Dict

class HTMLElement(Any): # ! Placeholder for actual HTMLElement type. This HTML element should take a context value as prop.
    value: Any

class Ref(Any): # ! Placeholder for actual Ref type.
    current: Any

State = Any
Reducer = Callable[[State, Dict[str, Any]], State]

def useReducer(reducer: Reducer, initialArg: Any, init: Callable[[Any], State]=lambda x: x) -> tuple[State, Callable[[Any], None]]:
    """
    A simple implementation of a reducer hook similar to React's useReducer.

    Args:
        reducer (callable): A function that takes the current state and an action, and returns the new state.
        initialArg (any): The initial argument to be passed to the init function.
        init (callable, optional): A function to initialize the state. Defaults to identity function.
    Returns:
        tuple: A tuple containing the current state and a dispatch function to send actions to the reducer.
    """
    state = init(initialArg)

    def dispatch(action: Dict[str, Any]) -> None:
        nonlocal state
        state = reducer(state, action)

    return state, dispatch


def useState(initialValue: State) -> tuple[State, Callable[[State], None]]:
    """
    A simple implementation of a state hook similar to React's useState.

    Args:
        initialValue (any): The initial state value.

    Returns:
        tuple: A tuple containing the current state and a function to update it.
    """
    state = initialValue

    def setState(newValue: Any) -> None:
        nonlocal state
        state = newValue

    return state, setState

# def createContext(defaultValue: Any) -> HTMLElement: # ! Placeholder for actual createContext function since it cannot be implemented now.
#    pass

def useContext(context: HTMLElement) -> Any:
    """
    A simple implementation of a context consumer hook similar to React's useContext.

    Args:
        context (HTMLElement): The context element to consume.

    Returns:
        any: The current context value.
    """
    return context.value

def useRef(initialValue: Any) -> Ref: # ! Placeholder for actual useRef function since it cannot be implemented now.
    """
    A simple implementation of a ref hook similar to React's useRef.

    Args:
        initialValue (any): The initial value for the ref.

    Returns:
        dict: A dictionary with a single key 'current' pointing to the ref value.
    """
    return Ref(current=initialValue)