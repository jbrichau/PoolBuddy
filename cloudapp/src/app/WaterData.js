import React from 'react';
import WaterChart from './WaterChart.js';
import request from 'superagent';

class WaterData extends React.Component {
  constructor(props) {
    super(props);
    this.state = { data: new Array() };
  }

  componentDidMount() {
    this.fetchData();
  }

  fetchData() {
    request.post('/pooldata')
    .set('Content-Type', 'application/json')
    .send({})
    .end((err,res) => { this.setState({"data":res.body[2]});});
  }

  render() {
    if(!this.state.data)
      return (<h1> Data fetch error </h1>);
    else if (this.state.data.length == 0)
      return (<h1> Waiting for pool data... </h1>);
    else return (
      <div>
        <WaterChart title="Temperature" unit="°c" dataKey="temperature" stroke="#8884d8" data={this.state.data} />
        <WaterChart title="Ph" unit="" dataKey="ph" minValue={7.2} maxValue={7.8} stroke="#82ca9d" data={this.state.data} />
        <WaterChart title="ORP" unit="mV" dataKey="orp" minValue={650} maxValue={800} stroke="#82ca9d" data={this.state.data} />
        <WaterChart title="WiFi" unit="" dataKey="wifi" stroke="#82ca9d" data={this.state.data} />
        <WaterChart title="Battery" unit="%" dataKey="soc" stroke="#82ca9d" data={this.state.data} />
     </div>);
  }
}

export default WaterData;
