import React, { Component, PropTypes } from 'react';
import { Router, Route, IndexRoute, Link } from 'react-router';
import {Menu} from 'antd';
import styles from './MainLayout.less';

const handleClick = function(item) {
  switch (item.key) {
    case "home":
      window.location.href = "/";
      break;
    case "board":
      window.location.href = "/board.html";
      break;
    case "manual":
      window.location.href = "/manual.html";
      break;
    case "scene":
      window.location.href = '/scene.html';
  }
}

const MainLayout = ({ children, activeKey }) => {
  return (
    <div>
      <Menu
        theme="dark"
        mode="horizontal"
        selectedKeys={[activeKey]}
        onClick={handleClick}
        >
        <Menu.Item key="home">首页</Menu.Item>
        <Menu.Item key="board">面板</Menu.Item>
        <Menu.Item key="manual">手动</Menu.Item>
        <Menu.Item key="scene">场景</Menu.Item>
      </Menu>
      <div>
        {children}
      </div>
    </div>
  );
};

MainLayout.propTypes = {
  children: PropTypes.node.isRequired,
  activeKey: PropTypes.string.isRequired
};

export default MainLayout;
