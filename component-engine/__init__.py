"""
Component Engine
A declarative UI component engine for Python, inspired by React but built with a C++ core.
"""

__version__ = "0.1.0"

from .component import Component
from .properties import Properties

__all__ = ["Component", "Properties"]
