import React from 'react';
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
    request.post('/data')
    .set('Content-Type', 'application/json')
    .send({})
    .end((err,res) => { this.setState({data:res.body[2]});});
  }

  render() {
    if (this.state.data.length == 0)
      return (<h1> Hello </h1>);
    else return (
      <div>
      	<LineChart width={1200} height={200} data={this.state.data}
              margin={{top: 5, right: 30, left: 20, bottom: 5}}>
         <XAxis dataKey="timestamp"/>
         <YAxis/>
         <CartesianGrid strokeDasharray="3 3"/>
         <Tooltip/>
         <Legend />
         <Line type="monotone" dataKey="temperature" stroke="#8884d8" activeDot={{r: 8}}/>
       </LineChart>
       <LineChart width={1200} height={200} data={this.state.data}
             margin={{top: 5, right: 30, left: 20, bottom: 5}}>
        <XAxis dataKey="timestamp"/>
        <YAxis/>
        <CartesianGrid strokeDasharray="3 3"/>
        <Tooltip/>
        <Legend />
        <Line type="monotone" dataKey="ph" stroke="#82ca9d" />
      </LineChart>
      <LineChart width={1200} height={200} data={this.state.data}
            margin={{top: 5, right: 30, left: 20, bottom: 5}}>
       <XAxis dataKey="timestamp"/>
       <YAxis/>
       <CartesianGrid strokeDasharray="3 3"/>
       <Tooltip/>
       <Legend />
       <Line type="monotone" dataKey="orp" stroke="#82ca9d" />
     </LineChart>
     <LineChart width={1200} height={200} data={this.state.data}
           margin={{top: 5, right: 30, left: 20, bottom: 5}}>
      <XAxis dataKey="timestamp"/>
      <YAxis/>
      <CartesianGrid strokeDasharray="3 3"/>
      <Tooltip/>
      <Legend />
      <Line type="monotone" dataKey="wifi" stroke="#82ca9d" />
    </LineChart>
    <LineChart width={1200} height={200} data={this.state.data}
          margin={{top: 5, right: 30, left: 20, bottom: 5}}>
     <XAxis dataKey="timestamp"/>
     <YAxis/>
     <CartesianGrid strokeDasharray="3 3"/>
     <Tooltip/>
     <Legend />
     <Line type="monotone" dataKey="soc" stroke="#82ca9d" />
   </LineChart>
     </div>);
  }
}

export default WaterData;
