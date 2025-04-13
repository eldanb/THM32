import { hydrate, prerender as ssr } from "preact-iso";
import { useEffect, useMemo, useState } from "preact/hooks";
import { useFormUtils } from "./form-utils";
import "./style.css";
import {
  ClockConfig,
  getConfig,
  getSsids,
  setConfig,
  SSIDInfo,
} from "./wordclock-api";

export function App() {
  const { state, setState, bindFormCheckbox, bindFormControls } =
    useFormUtils<ClockConfig>();

  const [ssidList, setSsidList] = useState<SSIDInfo[]>([]);
  const [loadingSsid, setLoadingSsid] = useState(false);

  const updateSsidsList = async () => {
    setLoadingSsid(true);
    setSsidList((await getSsids()).ssids);
    setLoadingSsid(false);
  };

  const loadConfig = async () => {
    setState(await getConfig());
  };

  useEffect(() => {
    loadConfig();
    updateSsidsList();
  }, []);

  const normalizedSsdList = useMemo(() => {
    const ret = [...ssidList];
    if (state?.ssid && ret.findIndex((v) => v.ssid === state.ssid) < 0) {
      ret.push({ ssid: state.ssid, rssi: null, enc: 0 });
    }

    return ret;
  }, [ssidList, state]);

  const handleConfigSave = async () => {
    await setConfig(state);
  };

  return (
    <div className="form-container">
      <h1>Word Clock Configuration</h1>
      {!state && "Loading..."}
      {state && (
        <>
          <div className="form-block">
            <h1>Network</h1>
            <table>
              <tr>
                <td>SSID:</td>
                <td>
                  <div className="horizontal-stack">
                    <select {...bindFormControls("ssid")}>
                      {normalizedSsdList.map((ssid) => (
                        <option value={ssid.ssid}>
                          {ssid.ssid} {ssid.rssi ? `(${ssid.rssi}db)` : ""}
                        </option>
                      ))}
                    </select>
                    {loadingSsid && <div>&nbsp;Scanning...</div>}
                  </div>
                </td>
              </tr>
              <tr>
                <td>Password:</td>
                <td>
                  <input type="password" {...bindFormControls("password")} />
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
                    {...bindFormControls(
                      "timeZone",
                      (control: HTMLSelectElement) => parseInt(control.value)
                    )}
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
                    {...bindFormCheckbox("isDst")}
                  />
                  &nbsp;
                  <label for="dstCheckbox">Daylight Saving Time</label>
                </td>
              </tr>
            </table>
          </div>

          <div className="form-block">
            <h1>Display</h1>
            <table>
              <tr>
                <td>Brightness:</td>
                <td>
                  <div className="horizontal-stack">
                    <input
                      type="number"
                      min={0}
                      max={100}
                      {...bindFormControls(
                        "brightness",
                        (control: HTMLInputElement) => parseInt(control.value)
                      )}
                    />
                    <div>%</div>
                  </div>
                </td>
              </tr>
              <tr>
                <td>Update period:</td>
                <td>
                  <div className="horizontal-stack">
                    <input
                      type="number"
                      min={15}
                      max={300}
                      {...bindFormControls(
                        "refreshPeriod",
                        (control: HTMLInputElement) => parseInt(control.value)
                      )}
                    />
                    <div>&nbsp;seconds</div>
                  </div>
                </td>
              </tr>
            </table>
          </div>

          <div className="form-commands">
            <div className="spacer">&nbsp;</div>
            <button onClick={handleConfigSave}>Save</button>
          </div>
        </>
      )}

      {import.meta.env.DEV && JSON.stringify(state)}
    </div>
  );
}

if (typeof window !== "undefined") {
  hydrate(<App />, document.getElementById("app"));
}

export async function prerender(data) {
  return await ssr(<App {...data} />);
}
