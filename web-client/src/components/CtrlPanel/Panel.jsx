import React, {Component, PropTypes} from 'react';
import { connect } from 'react-redux';
import { Link } from 'react-router';
import { Row, Col, Menu } from 'antd';
import MainLayout from '../../layouts/MainLayout/MainLayout';
import ListContainer from './ListContainer';
import actions from '../../actions/device';
import _ from 'lodash';

class Panel extends Component {
  constructor(props) {
    super(props);
    this.handleChangeAppProps = this._handleChangeAppProps.bind(this);
  }
  _handleChangeAppProps(nwk, ep, props) {
    this.props.onChangeAppProps(nwk, ep, props);
  }
  filterAppsByTypes(types) {
    const { devices } = this.props;
    return _.chain(devices)
      .map(dev => {
        const injectedApps = _.map(dev.apps, a => ({...a, nwk: dev.nwk, ieee: dev.ieee}));
        return injectedApps;
      })
      .flatten()
      .filter(app => !!(types.indexOf(app.type) >= 0))
      .value();
  }
  render() {
    const { type: appType } = this.props.params;
    const { devices, loading, err } = this.props;
    const apps = this.filterAppsByTypes([appType]);
    return (
      <MainLayout activeKey="manual">
        <Menu mode="horizontal" selectedKeys={[appType]}>
          <Menu.Item key={'lamp'}>
            <Link to="/ctrl/lamp">光源</Link>
          </Menu.Item>
          <Menu.Item key={'pulse'}>
            <Link to="/ctrl/pulse">轻触开关</Link>
          </Menu.Item>
          <Menu.Item key={'sensor-light'}>
            <Link to="/ctrl/sensor-light">光照传感器</Link>
          </Menu.Item>
        </Menu>
        <ListContainer
          appType={appType}
          apps={apps}
          loading={loading}
          onChangeAppProps={this.handleChangeAppProps}
        />
      </MainLayout>
    )
  }
}
Panel.PropTypes = {
  devices: PropTypes.array.isRequired,
  loading: PropTypes.bool.isRequired,
  err: PropTypes.any.isRequired,
  onChangeAppProps: PropTypes.func.isRequired,
}

module.exports = connect(
  state => ({
    devices: state.device.devices,
    loading: state.device.loading,
    err: state.device.err,
  }),
  dispatch => ({
    onChangeAppProps: (nwk, ep, props) => {
      dispatch(actions['device/app/prop/set'](nwk, ep, props))
    }
  })
)(Panel);
