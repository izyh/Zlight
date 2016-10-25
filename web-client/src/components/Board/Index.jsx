import React, {Component, PropTypes} from 'react';
import { connect } from 'react-redux';
import { Link } from 'react-router';
import { Row, Col, Card } from 'antd';
import MainLayout from '../../layouts/MainLayout/MainLayout';
import _ from 'lodash';
import actions from '../../actions'

function sysMode2Name(mode) {
  switch (mode) {
    case 'manual': return '手动';
    case 'staticScene': return '静态场景';
    case 'autoScene': return '自动场景';
    default: return mode;
  }
}

class Index extends Component {
  render() {
    const { devices, sysMode } = this.props;
    return (
      <MainLayout activeKey="board">
        <Card title="设备统计">
          <p>设备总数：{devices.length}</p>
        </Card>
        <Card title="系统">
          <p>当前模式：{sysMode2Name(sysMode)}</p>
        </Card>
      </MainLayout>
    )
  }
}

Index.propTypes = {
  devices: PropTypes.array.isRequired,
  sysMode: PropTypes.string.isRequired,
}

module.exports = connect(
  state => ({
    devices: state.device.devices,
    sysMode: state.mode.mode
  })
)(Index);