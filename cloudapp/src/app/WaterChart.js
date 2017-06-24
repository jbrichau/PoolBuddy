import React from 'react';
import {LineChart,XAxis,YAxis,Legend,Tooltip,CartesianGrid,Line,ReferenceLine} from 'recharts';

const CustomizedAxisTick = React.createClass({
  render () {
    const {x, y, stroke, payload} = this.props;

   	return (
    	<g transform={`translate(${x},${y})`}>
        <text x={0} y={0} dy={16} textAnchor="end" fill="#666">{new Date(payload.value).toLocaleString('nl-BE')}</text>
      </g>
    );
  }
});

class WaterChart extends React.Component {
  render() {
    let minReferenceLine;
    if(this.props.minValue)
      minReferenceLine = <ReferenceLine y={this.props.minValue} label="Min" stroke="red"/>;
    return (
      <LineChart width={1200} height={200} data={this.props.data} margin={{top: 5,right: 30,left: 20,bottom: 5}}>
        <XAxis dataKey="timestamp" scale="utcTime" tick={<CustomizedAxisTick/>}/>
        <YAxis/>
        <CartesianGrid strokeDasharray="3 3"/>
        <Tooltip/>
        <Legend/>
        <Line type="monotone" dataKey={this.props.dataKey} stroke={this.props.stroke} strokeWidth="2" dot={false} activeDot={{r: 8}}/>
        {minReferenceLine}
      </LineChart>
    );
  }
}

export default WaterChart;
