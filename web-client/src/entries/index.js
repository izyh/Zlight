import './index.html';
import React from 'react';
import ReactDOM from 'react-dom';
import './index.less';


let render = () => {
  const Home = require('../components/Home');
  ReactDOM.render(
    <Home />
  , document.getElementById('root'));
};

render();
