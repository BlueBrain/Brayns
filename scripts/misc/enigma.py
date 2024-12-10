from __future__ import annotations

from dataclasses import dataclass, field


@dataclass
class Component:
    coefficient: int
    exponent: list[Component] = field(default_factory=list)


def decompose(value: int, base: int) -> list[int]:
    if base < 2:
        raise ValueError(f"Base cannot be < 2: {base}")
    result = list[int]()
    while True:
        coefficient = value % base
        result.append(coefficient)
        if value < base:
            return result
        value //= base


def get_components(value: int, base: int) -> list[Component]:
    coefficients = decompose(value, base)
    if len(coefficients) == 1:
        return [Component(value)]
    return [
        Component(coefficient, get_components(exponent, base))
        for exponent, coefficient in enumerate(coefficients)
        if coefficient != 0
    ]


def evaluate(components: list[Component], base: int) -> int:
    if not components:
        return 0
    return sum(
        component.coefficient * base ** evaluate(component.exponent, base)
        for component in components
    )


def turtle(value: int) -> int:
    return value - 1


def achilles(value: int, step: int) -> int:
    components = get_components(value, step)
    return evaluate(components, step + 1)


def test() -> None:
    for value in range(1000):
        for base in range(2, 1000):
            components = get_components(value, base)
            assert evaluate(components, base) == value


def main() -> None:
    value = 4
    step = 2
    while value != 0:
        print(f"Begin: {step=} {value=}")
        value = achilles(value, step)
        print(f"Achilles: {step=} {value=}")
        value = turtle(value)
        print(f"Turtle: {step=} {value=}")
        step += 1


if __name__ == "__main__":
    main()
