/*!
 * \file Address.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <string>

namespace OSSIA {

class Device;

template <typename T>
class Address {

private:

  // Constructors, destructor, assignment
  Address();
  Address(const Address&);
  ~Address();
  Address & operator= (const Address&);

public:

  typedef T value_type;

  // Factories
  template <typename U>
  Address addAddress(std::string) const;

  // Navigation
  bool isRoot() const;
  Address & getParent() const;
  Device & getDevice() const;

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const Address&) const;

  // Managing children
  void addChild(const Address&);
  bool removeChild(const Address&);

  // Accessors
  std::string getAddress();

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}

#endif /* ADDRESS_H_ */