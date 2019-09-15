import React from 'react';
import WaterChart from './WaterChart.js';
import request from 'superagent';

class WeatherData extends React.Component {
  constructor(props) {
    super(props);
    this.state = { data: new Array() };
  }

  componentDidMount() {
    this.fetchData();
  }

  fetchData() {
    request.post('/weatherdata')
    .set('Content-Type', 'application/json')
    .send({})
    .end((err,res) => { this.setState({data:res.body[2]});});
  }

  render() {
    if(!this.state.data)
      return (<h1> Data fetch error </h1>);
    else if (this.state.data.length == 0)
      return (<h1> Waiting for data weather data... </h1>);
    else return (
      <div>
        <WaterChart title="Temperature" unit="°c" dataKey="tempc" stroke="#8884d8" data={this.state.data} />
        <WaterChart title="Wind" unit="kmh" dataKey="windkmh" stroke="#82ca9d" data={this.state.data} />
        <WaterChart title="Wind 2m avg" unit="kmh" dataKey="windkmh2mavg" stroke="#82ca9d" data={this.state.data} />
        <WaterChart title="Pressure" unit="mbar" dataKey="pressure" stroke="#82ca9d" data={this.state.data} />
        <WaterChart title="Humidity" unit="%" dataKey="humidity" stroke="#82ca9d" data={this.state.data} />
     </div>);
  }
}

export default WeatherData;
