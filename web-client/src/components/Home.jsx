import React, { Component, PropTypes } from 'react';
import MainLayout from '../layouts/MainLayout/MainLayout';


class Home extends Component {
  render() {
    return (
      <MainLayout activeKey="home">
        <div style={{textAlign: 'center'}}>
          <h1>iLighting you</h1>
          <p>进入 <a href="/manual.html">控制面板</a></p>
        </div>
      </MainLayout>
    )
  }
}
Home.propTypes = {
};

export default Home;
