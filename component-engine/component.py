from .properties import Properties


class Component:
    """
    Base class for all UI components.
    """

    def __init__(self, properties: Properties) -> None:
        self.properties = properties

    def render(self) -> None:
        raise NotImplementedError("Render method must be implemented by subclasses.")
