import React from 'react';
import ReactDOM from 'react-dom';
import Tabs from 'react-bootstrap/Tabs';
import Tab from 'react-bootstrap/Tab';
import WaterData from './WaterData';
import SoilData from './SoilData';
import WeatherData from './WeatherData';

ReactDOM.render(
  <Tabs defaultActiveKey="Pool" id="uncontrolled-tab-example">
    <Tab eventKey="Pool" title="Pool">
      <WaterData />
    </Tab>
    <Tab eventKey="Soil" title="Soil">
      <SoilData />
    </Tab>
    <Tab eventKey="Weather" title="Weather">
      <WeatherData />
    </Tab>
  </Tabs>,
  document.getElementById('root')
);
