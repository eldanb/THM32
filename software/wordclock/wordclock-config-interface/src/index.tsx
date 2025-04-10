import { hydrate, prerender as ssr } from "preact-iso";
import { useEffect, useMemo, useState } from "preact/hooks";
import "./style.css";
import { ClockConfig, getConfig, getSsids, SSIDInfo } from "./wordclock-api";

export function App() {
  const [ssidList, setSsidList] = useState<SSIDInfo[]>([]);
  const [clockConfig, setClockConfig] = useState<ClockConfig>({
    password: "",
    timeZone: 2,
    isDst: false,
  });

  const updateSsidsList = async () => {
    setSsidList((await getSsids()).ssids);
  };

  const loadConfig = async () => {
    setClockConfig(await getConfig());
  };

  useEffect(() => {
    loadConfig();
    updateSsidsList();
  }, []);

  const normalizedSsdList = useMemo(() => {
    const ret = [...ssidList];
    if (
      clockConfig.ssid &&
      ret.findIndex((v) => v.ssid === clockConfig.ssid) < 0
    ) {
      ret.push({ ssid: clockConfig.ssid, rssi: "(unknown)", enc: 0 });
    }

    return ret;
  }, [ssidList, clockConfig]);

  return (
    <div className="form-container">
      <h1>Word Clock Configuration</h1>
      <div className="form-block">
        <h1>Network</h1>
        <table>
          <tr>
            <td>SSID:</td>
            <td>
              <div className="horizontal-stack">
                <select
                  value={clockConfig.ssid}
                  onChange={(event) =>
                    setClockConfig((config) => ({
                      ...config,
                      ssid: event.currentTarget.value,
                    }))
                  }
                >
                  {normalizedSsdList.map((ssid) => (
                    <option value={ssid.ssid}>
                      {ssid.ssid} ({ssid.rssi}db)
                    </option>
                  ))}
                </select>
                <div>bla</div>
              </div>
            </td>
          </tr>
          <tr>
            <td>Password:</td>
            <td>
              <input
                type="password"
                value={clockConfig.password}
                onChange={(event) =>
                  setClockConfig((config) => ({
                    ...config,
                    password: event.currentTarget.value,
                  }))
                }
              />
            </td>
          </tr>
        </table>
      </div>

      <div className="form-block">
        <h1>Time</h1>
        <table>
          <tr>
            <td>Timezone:</td>
            <td>
              <select
                value={clockConfig.timeZone}
                onChange={(event) =>
                  setClockConfig((config) => ({
                    ...config,
                    timeZone: parseInt(event.currentTarget.value),
                  }))
                }
              >
                {[...Array(24).keys()].map((tzIndex) => (
                  <option value={tzIndex - 12}>
                    GMT{tzIndex >= 12 ? "+" : ""}
                    {tzIndex - 12}
                  </option>
                ))}
              </select>
            </td>
          </tr>
          <tr>
            <td>&nbsp;</td>
            <td>
              <input
                type="checkbox"
                id="dstCheckbox"
                checked={clockConfig.isDst}
                onChange={(event) =>
                  setClockConfig((config) => ({
                    ...config,
                    isDst: event.currentTarget.checked,
                  }))
                }
              />
              &nbsp;
              <label for="dstCheckbox">Daylight Saving Time</label>
            </td>
          </tr>
        </table>
      </div>

      <div className="form-commands">
        <div className="spacer">&nbsp;</div>
        <button>Save</button>
      </div>

      {JSON.stringify(clockConfig)}
    </div>
  );
}

if (typeof window !== "undefined") {
  hydrate(<App />, document.getElementById("app"));
}

export async function prerender(data) {
  return await ssr(<App {...data} />);
}
