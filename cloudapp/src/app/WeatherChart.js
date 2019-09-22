import React from 'react';
import { Badge } from 'reactstrap';
import { Charts, ChartContainer, ChartRow, YAxis, LineChart, styler } from "react-timeseries-charts";

class WeatherChart extends React.Component {
    render() {
      return (
        <div>
            <h2>{this.props.title} <Badge color="secondary">{this.props.data.atLast().toJSON().data[this.props.dataKey].toFixed(2)} {this.props.unit}</Badge></h2>
            <ChartContainer timeRange={this.props.data.timerange()} width={1000}>
                <ChartRow height="200">
                <YAxis id={this.props.dataKey} label={this.props.title+' ('+this.props.unit+')'} labelOffset={-5} min={this.props.data.min(this.props.dataKey)} max={this.props.data.max(this.props.dataKey)} type="linear" format=",.1f"/>
                <Charts>
                    <LineChart axis={this.props.dataKey} series={this.props.data} columns={[this.props.dataKey]} style={styler([{key: this.props.dataKey, color: this.props.stroke, strokeWidth: 4 }])}/>
                </Charts>
                </ChartRow>
            </ChartContainer>
        </div>
      );
    }
  }
  
  export default WeatherChart;
  