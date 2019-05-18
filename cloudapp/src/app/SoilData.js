
import React from 'react';
import WaterChart from './WaterChart.js';
import request from 'superagent';

class SoilData extends React.Component {
  constructor(props) {
    super(props);
    this.state = { data: new Array() };
  }

  componentDidMount() {
    this.fetchData();
  }

  fetchData() {
    request.post('/soildata')
    .set('Content-Type', 'application/json')
    .send({})
    .end((err,res) => { this.setState({data:res.body[2]});});
  }

  render() {
    if(!this.state.data)
      return (<h1> Data fetch error </h1>);
    else if (this.state.data.length == 0)
      return (<h1> Waiting for soil data... </h1>);
    else return (
        <WaterChart title="Humidity" unit="%" dataKey="soilhumidity" stroke="#8884d8" data={this.state.data} />
      )
  }
}

export default SoilData;
