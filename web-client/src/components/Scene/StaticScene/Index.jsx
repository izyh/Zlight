import React, {Component, PropTypes} from 'react';
import { connect } from 'react-redux';
import { Link } from 'react-router';
import { Row, Col, Menu } from 'antd';
import _ from 'lodash';
import Meta from './Meta';
import SceneEditor from './SceneEditor';
import actions from '../../../actions'

class Index extends Component {
  constructor(props) {
    super(props);
    this.state = {
      currentSceneId: ''
    }
  }
  get currentScene() {
    return _.find(this.props.sceneList, item => item.id==this.state.currentSceneId)
  }
  handleSelectScene(e) {
    this.setState({
      currentSceneId: e.key
    })
  }
  handleMetaChange(props) {
    const { name } = props;
    this.props.onSceneChange && this.props.onSceneChange(Object.assign({},this.currentScene, {
      name
    }))
  }
  render() {
    const { currentScene } = this;
    const { sceneList } = this.props;
    const { currentSceneId } = this.state;
    return (
      <Row>
        <Col span={4}>
          <Menu 
            onClick={this.handleSelectScene.bind(this)}
            selectKeys={[currentSceneId]}
          >
            {sceneList.map(item => (
              <Menu.Item key={item.id}>{item.name}</Menu.Item>
            ))}
          </Menu>
        </Col>
        <Col span={20}>
          {
            currentScene ? 
            <div>
              <Meta name={currentScene.name} onChange={this.handleMetaChange.bind(this)} />
              <SceneEditor
                sceneItems={currentScene.items}
              />
            </div> :
            <div>请选择一个场景</div>
          }
        </Col>
      </Row>
    )
  }
}

Index.propTypes = {
  sceneList: PropTypes.array.isRequired,
  onSceneChange: PropTypes.func,
}

module.exports = connect(
  state => ({
    sceneList: state.scene.sSceneList
  }),
  dispatch => ({
    onSceneChange: scene => {
      dispatch(actions['scene/modify/static'](scene))
    }
  })
)(Index);