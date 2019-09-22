import React from 'react';
import WeatherChart from './WeatherChart.js';
import request from 'superagent';
import { TimeSeries } from 'pondjs';

class WeatherData extends React.Component {
  constructor(props) {
    super(props);
    this.state = { data: new TimeSeries({
      name: "weather",
      columns: ["time", "tempc","windkmh","windkmh2mavg","pressure","humidity"], points: []})};
  }

  componentDidMount() {
    this.fetchData();
  }

  fetchData() {
    request.post('/weatherdata')
    .set('Content-Type', 'application/json')
    .send({})
    .end((err,res) => { 
      const timeseries = new TimeSeries({
        name: "weather",
        columns: ["time", "tempc","windkmh","windkmh2mavg","pressure","humidity"],
        points: res.body[2].map((d) => [ d.timestamp = new Date(d.timestamp).getTime(),d.tempc,d.windkmh,d.windkmh2mavg,d.pressure,d.humidity ])});
      this.setState({data:timeseries}); });
  }

  render() {
    if(!this.state.data)
      return (<h1> Data fetch error </h1>);
    else if (this.state.data.size() == 0)
      return (<h1> Waiting for weather data... </h1>);
    else return (
      <div>
        <WeatherChart title="Temperature" unit="°C" dataKey="tempc" stroke="#8884d8" data={this.state.data} />
        <WeatherChart title="Wind" unit="kmh" dataKey="windkmh" stroke="#82ca9d" data={this.state.data} />
        <WeatherChart title="Wind 2m avg" unit="kmh" dataKey="windkmh2mavg" stroke="#82ca9d" data={this.state.data} />
        <WeatherChart title="Pressure" unit="mbar" dataKey="pressure" stroke="#82ca9d" data={this.state.data} />
        <WeatherChart title="Humidity" unit="%" dataKey="humidity" stroke="#82ca9d" data={this.state.data} />
     </div>);
  }
}

export default WeatherData;
