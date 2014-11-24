/*!
 * \file DeviceOSC.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Network/Device.h"
#include "Network/Protocols.h"

#include "TTModular.h"

#include "Device.cpp"

namespace OSSIA {
  
template <>
class Device<OSC>::Impl : public SharedImpl {
    
  public:
    
    Impl(std::string name)
    {
      // create distante application
      TTErr   err;
      TTValue out;
      err = mApplicationManager.send("ApplicationInstantiateDistante", name.data(), out);
      
      // TODO : register the local application to the OSC protocol
      
      if (err) {
        TTLogError("Error : can't create %s application\n", name.data());
        return;
      }
      else
        mApplication = out[0];
      
      // set the application in debug mode
      mApplication.set("debug", YES);
    };
    
    bool registerDataAtAddress(TTAddress& anAddress, TTSymbol& service, TTObject& returnedData)
    {
      // create a proxy data
      TTValue out, v(anAddress, service);
      if (!mApplication.send("ProxyDataInstantiate", v, out)) {
        
        returnedData = out[0];
        
        // initialize the value with a default 0. value
        returnedData.set("valueDefault", 0.);
        
        returnedData.send("Init");
        
        return true;
      }
      
      return false;
    }
  };

template <>
template <typename U>
Address<U> Device<OSC>::addAddress(std::string address) const
{
  TTAddress anAddress(address.data());
  TTSymbol  service("parameter");
  TTObject  aData;
  
  if (pimpl->registerDataAtAddress(anAddress, service, aData))
  {
    // TODO : setup attribute for OSC protocol case and depending on the U type
  }
}

template <>
template <typename U>
Address<U> Device<OSC>::addAddress(std::string address, U min, U max) const
{
  TTAddress anAddress(address.data());
  TTSymbol  service("parameter");
  TTObject  aData;
  
  if (pimpl->registerDataAtAddress(anAddress, service, aData))
  {
    // TODO : setup attribute for OSC protocol case depending on the U type
  }
}

}