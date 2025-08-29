from .component_properties import ComponentProperties


class Component:
    """
    Base class for all UI components.
    """

    def __init__(self, properties: ComponentProperties) -> None:
        self.properties = properties
