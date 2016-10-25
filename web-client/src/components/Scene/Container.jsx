import React, {Component, PropTypes} from 'react';
import { connect } from 'react-redux';
import { Link } from 'react-router';
import { Row, Col, Menu } from 'antd';
import MainLayout from '../../layouts/MainLayout/MainLayout';


class Container extends Component {
  render() {
    return (
      <MainLayout activeKey="scene">
        <Menu mode="horizontal" selectedKeys={['static']}>
          <Menu.Item key={'static'}>
            <Link to="/static">静态场景</Link>
          </Menu.Item>
          <Menu.Item key={'auto'}>
            <Link to="/auto">动态场景</Link>
          </Menu.Item>
        </Menu>
        {this.props.children}
      </MainLayout>
    )
  }
}
Container.propTypes = {
  children: PropTypes.node.isRequired
}

// module.exports = connect(
//   state => ({
//     devices: state.device.devices,
//     loading: state.device.loading,
//     err: state.device.err,
//   }),
//   dispatch => ({
//     onChangeAppProps: (nwk, ep, props) => {
//       dispatch(actions['device/app/prop/set'](nwk, ep, props))
//     }
//   })
// )(Panel);

module.exports = Container;
