export type ClockConfig = {
  ssid?: string;
  password: string;
  timeZone: number;
  isDst: boolean;

  refreshPeriod: number;
  brightness: number;
};

export type SSIDInfo = {
  ssid: string;
  rssi: string;
  enc: number;
};

function getApiEndpointUrl(url: string): string {
  return (
    (import.meta.env.VITE_WORDCLOCK_HOST
      ? `http://${import.meta.env.VITE_WORDCLOCK_HOST}`
      : "") + url
  );
}

async function invokeGetApi<TResult>(url: string): Promise<TResult> {
  const response = await fetch(getApiEndpointUrl(url));
  return (await response.json()) as TResult;
}

async function invokePostApi<TResult>(
  url: string,
  body: any
): Promise<TResult> {
  const response = await fetch(getApiEndpointUrl(url), {
    body: JSON.stringify(body),
    headers: {
      "Content-Type": "application/json",
    },
    method: "POST",
  });
  return (await response.json()) as TResult;
}

export function getConfig() {
  return invokeGetApi<ClockConfig>("/config");
}

export function setConfig(config: ClockConfig) {
  return invokePostApi<{}>("/config", config);
}

export function getSsids() {
  return invokeGetApi<{ ssids: SSIDInfo[] }>("/ssids");
}
