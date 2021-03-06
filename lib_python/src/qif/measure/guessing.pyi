"""
Guessing entropy.
"""
from .. import typing as t

def add_leakage(pi: t.ndarray, C: t.ndarray) -> t.FloatOrRat: ...

def mult_leakage(pi: t.ndarray, C: t.ndarray) -> t.FloatOrRat: ...

def posterior(pi: t.ndarray, C: t.ndarray) -> t.FloatOrRat: ...

def prior(pi: t.ndarray) -> t.FloatOrRat: ...
