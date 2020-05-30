import React from 'react';
import request from 'superagent';
import { TimeSeries } from 'pondjs';
import { Resizable, Charts, ChartContainer, ChartRow, YAxis, ValueAxis, LineChart, styler } from "react-timeseries-charts";

class WeatherData extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      data: new TimeSeries({
        name: "weather",
        columns: [
          "time",
          "tempc",
          "windkmh",
          "windkmh2mavg",
          "pressure",
          "humidity",
        ],
        points: [],
      }),
    };
  }

  componentDidMount() {
    this.fetchData();
  }

  fetchData() {
    request
      .post("/weatherdata")
      .set("Content-Type", "application/json")
      .send({})
      .end((err, res) => {
        const timeseries = new TimeSeries({
          name: "weather",
          columns: [
            "time",
            "tempc",
            "windkmh",
            "windkmh2mavg",
            "windgustkmh10m",
            "pressure",
            "humidity",
            "dailyrainmm",
          ],
          points: res.body[2].map((d) => [
            (d.timestamp = new Date(d.timestamp).getTime()),
            d.tempc,
            d.windkmh,
            d.windkmh2mavg,
            d.windgustkmh10m,
            d.pressure,
            d.humidity,
            d.dailyrainmm,
          ]),
        });
        this.setState({ data: timeseries, tracker: null });
      });
  }

  handleTrackerChanged = (t) => {
    this.setState({ tracker: t });
  };

  renderChart(dataKey, label, color) {
    // Get the value at the current tracker position for the ValueAxis
    let value = "--";
    let v = null;
    if (this.state.tracker)
      v = this.state.data.atTime(this.state.tracker).get(dataKey);
    else v = this.state.data.atLast().get(dataKey);
    if (v) 
      value = parseInt(v, 10);
    else if (v==0) value = "0";
    return (
      <ChartRow height="100">
        <YAxis
          id={dataKey}
          label={label}
          labelOffset={-5}
          min={this.state.data.min(dataKey)}
          max={this.state.data.max(dataKey)}
          type="linear"
          format=",.1f"
        />
        <Charts>
          <LineChart
            axis={dataKey}
            series={this.state.data}
            columns={[dataKey]}
            style={styler([{ key: dataKey, color: color, strokeWidth: 4 }])}
          />
        </Charts>
        <ValueAxis
          id={`${dataKey}_valueaxis`}
          visible={true}
          value={value}
          detail={label}
          width={80}
          min={0}
          max={35}
        />
      </ChartRow>
    );
  }

  render() {
    if (!this.state.data) return <h1> Data fetch error </h1>;
    else if (this.state.data.size() == 0)
      return <h1> Waiting for weather data... </h1>;
    else
      return (
        <Resizable>
          <ChartContainer
            timeRange={this.state.data.timerange()}
            onTrackerChanged={this.handleTrackerChanged}
            trackerPosition={this.state.tracker} 
          >
            {this.renderChart("tempc", "Temp °C", "steelblue")}
            {this.renderChart("pressure", "Pressure mbar", "#F68B24")}
            {this.renderChart("humidity", "Humidity %", "#ff47ff")}
            {this.renderChart("windkmh", "Wind kmh", "green")}
            {this.renderChart("windkmh2mavg", "Wind avg kmh", "green")}
            {this.renderChart("windgustkmh10m", "Wind gust kmh", "green")}
            {this.renderChart("dailyrainmm", "Rain mm", "steelblue")}
          </ChartContainer>
        </Resizable>
      );
  }
}

export default WeatherData;