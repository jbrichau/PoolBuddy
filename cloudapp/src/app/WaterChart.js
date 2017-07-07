import React from 'react';
import { LineChart, XAxis, YAxis, Legend, Tooltip, CartesianGrid, Line, ReferenceLine } from 'recharts';

const CustomizedAxisTick = React.createClass({
  render() {
    const { x, y, stroke, payload } = this.props;

    return (
      <g transform={`translate(${x},${y})`}>
        <text x={0} y={0} dy={16} textAnchor="end" fill="#666">{new Date(payload.value).toLocaleString('nl-BE')}</text>
      </g>
    );
  }
});

function CustomizedTooltip(props) {
    const { active } = props;

    if (active) {
      const { payload, label } = props;
      return (
        <div className="recharts-default-tooltip" style={{'margin': '0px', 'padding': '10px', 'backgroundColor': 'rgb(255, 255, 255)', 'border': '1px solid rgb(204, 204, 204)', 'whiteSpace': 'nowrap'}}>
          <p className="recharts-tooltip-label" style={{'margin': '0px'}}>{new Date(label).toLocaleString('nl-BE')}</p>
          <ul className="recharts-tooltip-item-list" style={{'padding': '0px', 'margin': '0px'}}>
            <li className="recharts-tooltip-item" style={{'display': 'block', 'paddingTop': '4px', 'paddingBottom': '4px', 'color': payload[0].color}}>
              <span className="recharts-tooltip-item-name">{payload[0].name}</span>
              <span className="recharts-tooltip-item-separator"> : </span>
              <span className="recharts-tooltip-item-value">{payload[0].value}</span>
              <span className="recharts-tooltip-item-unit"></span>
              </li>
            </ul>   
        </div>
      );
    } else return null;
}

class WaterChart extends React.Component {
  render() {
    let minReferenceLine, maxReferenceLine;
    const ticks = new Array();
    this.props.data[this.props.data.length - 1]
    if (this.props.minValue)
      minReferenceLine = <ReferenceLine y={this.props.minValue} stroke="red" alwaysShow={true}/>;
    if (this.props.maxValue)
      maxReferenceLine = <ReferenceLine y={this.props.maxValue} stroke="red" alwaysShow={true}/>;
    return (
      <LineChart width={1200} height={200} data={this.props.data} margin={{ top: 5, right: 30, left: 20, bottom: 5 }}>
        <XAxis dataKey="timestamp" scale="utcTime" tick={<CustomizedAxisTick />} />
        <YAxis />
        <CartesianGrid strokeDasharray="3 3" />
        <Tooltip content={<CustomizedTooltip/>}/>
        <Legend />
        <Line type="monotone" dataKey={this.props.dataKey} stroke={this.props.stroke} strokeWidth="2" dot={false} activeDot={{ r: 8 }} />
        {minReferenceLine}
        {maxReferenceLine}
      </LineChart>
    );
  }
}

export default WaterChart;
