from typing import Dict, Optional


class Properties:
    """
    Class to manage properties for a UI component.
    """

    def __init__(self) -> None:
        self.properties: Dict[str, Optional[str | int | float | bool]] = {}

    def set_property(self, key: str, value: Optional[str | int | float | bool]) -> None:
        self.properties[key] = value

    def get_property(self, key: str) -> Optional[str | int | float | bool]:
        return self.properties.get(key)

    def remove_property(self, key: str) -> None:
        self.properties.pop(key, None)

    def clear_properties(self) -> None:
        self.properties.clear()