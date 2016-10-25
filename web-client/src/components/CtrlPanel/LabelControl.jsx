import React, {Component, PropTypes} from 'react';
import { Icon } from 'antd';
import _ from 'lodash';
import cls from 'classnames';
import styles from './LabelControl.less';


const PT = PropTypes;

class LabelControl extends Component {
  constructor(props) {
    super(props);
    this.handleEdit = this._handleEdit.bind(this);
  }
  _handleEdit() {
    this.props.onEdit && this.props.onEdit();
  }
  render() {
    const { label, show: showNode, edit: isEditing, children, loading } = this.props;
    const wrapCls = cls({
      'labelControl': true,
      'labelControl--editing': isEditing,
      'labelControl--loading': loading,
    });
    return (
      <div className={wrapCls}>
        <label className="labelControl-label">{label}:</label>
        <div className="labelControl-content">
          <div className="labelControl-content-show">{showNode}</div>
          <div className="labelControl-content-edit">{children}</div>
          <div className="labelControl-content-loading">
            <Icon type="loading" />
          </div>
        </div>
        <div className="labelControl-editBtn" onClick={this.handleEdit}>
          <Icon type="edit" />
        </div>
      </div>
    )
  }
}
LabelControl.propTypes = {
  label: PT.string.isRequired,
  loading: PT.bool.isRequired,
  edit: PT.bool.isRequired,
  show: PT.node.isRequired,
  onEdit: PT.func,
  children: PT.node
}

module.exports = LabelControl;
