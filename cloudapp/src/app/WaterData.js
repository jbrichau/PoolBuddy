import React from 'react';
import WaterChart from './WaterChart.js';
import {LineChart,XAxis,YAxis,Legend,Tooltip,CartesianGrid,Line} from 'recharts';
import request from 'superagent';

class WaterData extends React.Component {
  constructor(props) {
    super(props);
    this.state = { "data":[] };
  }

  componentDidMount() {
    this.fetchData();
  }

  fetchData() {
    request.post('/pooldata')
    .set('Content-Type', 'application/json')
    .send({})
    .end((err,res) => { this.setState({'data':res.body[2]});});
  }

  render() {
    if (this.state.data.length == 0)
      return (<h1> Waiting for data... </h1>);
    else return (
      <div>
        <WaterChart dataKey="temperature" stroke="#8884d8" data={this.state.data} />
        <WaterChart dataKey="ph" minValue={7.0} maxValue={7.8} stroke="#82ca9d" data={this.state.data} />
        <WaterChart dataKey="orp" minValue={650} maxValue={800} stroke="#82ca9d" data={this.state.data} />
        <WaterChart dataKey="wifi" stroke="#82ca9d" data={this.state.data} />
        <WaterChart dataKey="soc" stroke="#82ca9d" data={this.state.data} />
     </div>);
  }
}

export default WaterData;
