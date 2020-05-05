/**
 * @file
 * <argos3/plugins/simulator/visualizations/webviz/webviz_user_functions.h>
 *
 * @author Prajankya Sonar - <prajankya@gmail.com>
 *
 * @project ARGoS3-Webviz <https://github.com/NESTlab/argos3-webviz>
 *
 * MIT License
 * Copyright (c) 2020 NEST Lab
 */

#ifndef ARGOS_WEBVIZ_USER_FUNCTIONS_H
#define ARGOS_WEBVIZ_USER_FUNCTIONS_H

namespace argos {
  class CWebvizUserFunctions;
}  // namespace argos

#include <argos3/core/simulator/entity/entity.h>
#include <argos3/core/utility/configuration/base_configurable_resource.h>
#include <argos3/core/utility/plugins/factory.h>

#include <functional>
#include <nlohmann/json.hpp>

namespace argos {
  class CWebvizUserFunctions : public CBaseConfigurableResource {
   public:
    /**
     * Class constructor.
     */
    CWebvizUserFunctions();

    /**
     * Class destructor.
     */
    virtual ~CWebvizUserFunctions();

    virtual void Init(TConfigurationNode& t_tree) {}
    virtual void Reset() {}
    virtual void Destroy() {}

    /**
     * Send data hook to add extra content to JSON message sent to clients.
     * Return a JSON object which will be attached to JSON body as "extra_data"
     */
    virtual const nlohmann::json sendExtraData() { return nullptr; }

    /**
     * Registers a user method.
     * @param USER_IMPL A user-defined subclass of CWebvizUserFunctions.
     * @param ENTITY The entity type to pass as a parameter to the user-defined
     * method.
     * @param pt_function The actual user-defined pointer-to-method.
     */
    template <typename USER_IMPL, typename ENTITY>
    void RegisterWebvizUserFunction(
      const nlohmann::json (USER_IMPL::*pt_function)(ENTITY&));

    /**
     * Calls a user method for the given entity.
     * @param The method to pass as parameter.
     */
    virtual const nlohmann::json Call(CEntity& c_entity);

   protected:
    /**
     * Pointer-to-thunk type definition.
     * @see Thunk
     */
    typedef const nlohmann::json (CWebvizUserFunctions::*TThunk)(CEntity&);

    /**
     * A templetized thunk.
     * This is a trampoline function that internally performs the
     * dispatch to a user-defined method.
     * @param USER_IMPL A user-defined subclass of CWebvizUserFunctions.
     * @param ENTITY The entity type to pass as a parameter to the user-defined
     * method.
     * @param c_entity The entity to pass as parameter.
     * @see TThunk
     */
    template <typename USER_IMPL, typename ENTITY>
    const nlohmann::json Thunk(CEntity& c_entity);

    /**
     * The base function holder.
     * @see CFunctionHolderImpl
     */
    class CFunctionHolder {};

    /**
     * The actual function holder.
     * This template class holds a pointer to a user-defined method.
     * @param USER_IMPL A user-defined subclass of CWebvizUserFunctions.
     * @param ENTITY The entity type to pass as a parameter to the user-defined
     * method.
     * @see CFunctionHolder
     */
    template <typename USER_IMPL, typename ENTITY>
    class CFunctionHolderImpl : public CFunctionHolder {
     public:
      typedef const nlohmann::json (USER_IMPL::*TFunction)(ENTITY&);
      TFunction Function;
      CFunctionHolderImpl(TFunction t_function) : Function(t_function) {}
    };

    /**
     * The vtable storing the thunks.
     * @see TThunk
     * @see Thunk
     */
    CVTable<CWebvizUserFunctions, CEntity, TThunk> m_cThunks;

    /**
     * A vector of function holders.
     * @see CFunctionHolder
     */
    std::vector<CFunctionHolder*> m_vecFunctionHolders;
  };

  /****************************************/
  /****************************************/

  template <typename USER_IMPL, typename ENTITY>
  const nlohmann::json CWebvizUserFunctions::Thunk(CEntity& c_entity) {
    /*
     * When this method is called, the static type of 'this'
     * is CWebvizUserFunctions. Since we want to call
     * method in USER_IMPL (subclass of CWebvizUserFunctions),
     * we need a cast. The cast is static because we trust
     * the user on not doing anything stupid.
     * The variable cImpl can be static because the cast is necessary
     * only the first time this function is called.
     */
    static USER_IMPL& cImpl = static_cast<USER_IMPL&>(*this);

    /* Cast the argument to the right type */
    const ENTITY& cEntity = static_cast<ENTITY&>(c_entity);

    /* Cast the function holder to its effective type */
    CFunctionHolderImpl<USER_IMPL, ENTITY>& cFunctionHolder =
      static_cast<CFunctionHolderImpl<USER_IMPL, ENTITY>&>(
        *m_vecFunctionHolders[GetTag<ENTITY, CEntity>()]);

    /* Call the user-defined method */
    return (cImpl.*(cFunctionHolder.Function))(cEntity);
  }

  /****************************************/
  /****************************************/

  template <typename USER_IMPL, typename ENTITY>
  void CWebvizUserFunctions::RegisterWebvizUserFunction(
    const nlohmann::json (USER_IMPL::*pt_function)(ENTITY&)) {
    /* Add the thunk to the VTable */
    m_cThunks.Add<ENTITY>(&CWebvizUserFunctions::Thunk<USER_IMPL, ENTITY>);
    /* Add the function holder to the vector, padding gaps with NULL pointers */
    size_t unIdx = GetTag<ENTITY, CEntity>();
    if (m_vecFunctionHolders.size() <= unIdx) {
      m_vecFunctionHolders.resize(unIdx + 1, NULL);
    }
    m_vecFunctionHolders[unIdx] =
      new CFunctionHolderImpl<USER_IMPL, ENTITY>(pt_function);
  }

  /****************************************/
  /****************************************/

}  // namespace argos

/* Definitions useful for dynamic linking of user functions */
#define REGISTER_WEBVIZ_USER_FUNCTIONS(CLASSNAME, LABEL) \
  REGISTER_SYMBOL(                                       \
    CWebvizUserFunctions,                                \
    CLASSNAME,                                           \
    LABEL,                                               \
    "undefined",                                         \
    "undefined",                                         \
    "undefined",                                         \
    "undefined",                                         \
    "undefined")

#endif
