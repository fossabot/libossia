#pragma once
#include <ossia/network/base/address.hpp>
#include <QObject>
#include <QQmlExpression>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <QQmlScriptString>
#include <ossia-qt/device/qml_node_base.hpp>
#include <ossia-qt/qml_context.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_device;
class qml_property_context : public QObject
{
  Q_OBJECT
public:
  qml_property_context(
      QQmlProperty& p, ossia::net::address_base& addr, QObject* parent);

  QQmlProperty& targetProperty;
  ossia::net::address_base& address;
public slots:
  void qtVariantChanged();
};

class qml_property_reader : public qml_node_base,
                            public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)

public:
  qml_property_reader(QQuickItem* parent = nullptr);
  ~qml_property_reader();

  void setTarget(const QQmlProperty& prop) override;
  void setDevice(QObject* device) override;
  void resetNode() override;

private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
  qml_property_context* m_propCtx{};
};

class qml_property_writer : public qml_node_base,
                            public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)

public:
  qml_property_writer(QQuickItem* parent = nullptr);
  ~qml_property_writer();

  void setTarget(const QQmlProperty& prop) override;
  void setDevice(QObject* device) override;
  void resetNode() override;

signals:
  void setValue_sig(const value&);

private slots:
  void setValue_slot(const value&);

private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
  ossia::net::address_base::iterator m_cb;
};

class qml_binding : public qml_node_base
{
  Q_OBJECT
  Q_PROPERTY(QQmlScriptString on READ on WRITE setOn NOTIFY onChanged)
public:
  qml_binding(QQuickItem* parent = nullptr);
  ~qml_binding();

  void setDevice(QObject* device) override;
  void resetNode() override;

  QQmlScriptString on() const;

public slots:
  void setOn(QQmlScriptString on);

signals:
  void onChanged(QQmlScriptString on);

private:
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  std::unique_ptr<QQmlExpression> m_expr;
  ossia::net::address_base* m_address{};
  QQmlScriptString m_on;
};

class qml_callback : public qml_node_base
{
  Q_OBJECT
  Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
public:
  qml_callback(QQuickItem* parent = nullptr);
  ~qml_callback();

  void setDevice(QObject* device) override;
  void resetNode() override;

  QVariant value() const;

signals:
  void valueChanged(QVariant);
  void setValue_sig(const ossia::value&);

public slots:
  void setValue(QVariant value);
  void setValue_slot(const ossia::value&);

private:
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  ossia::net::address_base* m_address{};
  QVariant m_value;
  ossia::net::address_base::iterator m_cb;
};
}
}
