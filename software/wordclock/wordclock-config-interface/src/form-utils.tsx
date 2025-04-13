import { ChangeEvent } from "preact/compat";
import { useState } from "preact/hooks";

export function useFormUtils<TState>(initialState?: TState) {
  const [state, setState] = useState<TState>(initialState);

  const bindFormControls = <
    T extends keyof TState,
    E extends EventTarget = HTMLInputElement
  >(
    field: T,
    transform: (v: E) => TState[T] = null
  ) => ({
    value: state[field],
    onChange: (event: ChangeEvent<E>) =>
      setState((config) => ({
        ...config,
        [field]: transform
          ? transform(event.currentTarget)
          : (event.currentTarget as any).value,
      })),
  });

  const bindFormCheckbox = <T extends keyof TState>(field: T) => ({
    checked: state[field],
    onChange: (event: ChangeEvent<HTMLInputElement>) =>
      setState((config) => ({
        ...config,
        [field]: event.currentTarget.checked,
      })),
  });

  return {
    state,
    setState,
    bindFormControls,
    bindFormCheckbox,
  };
}
