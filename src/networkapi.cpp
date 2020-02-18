#include "networkapi.h"
#include "helpers/utils.h"

namespace argos {

  /****************************************/
  /****************************************/

  CNetworkAPI::CNetworkAPI() : m_cTimer(), m_cSpace(m_cSimulator.GetSpace()) {
    m_cSimulationThread =
      std::thread(&CNetworkAPI::SimulationThreadFunction, this);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Init(TConfigurationNode& t_tree) {
    /* Setting up Logging */
    LOG_SCOPE_FUNCTION(INFO);

    unsigned short unPort;
    unsigned short unBroadcastFrequency;

    /* Parse options from the XML */
    GetNodeAttributeOrDefault(t_tree, "port", unPort, argos::UInt16(3000));
    GetNodeAttributeOrDefault(
      t_tree, "broadcast_frequency", unBroadcastFrequency, argos::UInt16(10));

    /* Initialize Webserver */
    m_cWebServer =
      new argos::NetworkAPI::CWebServer(this, unPort, unBroadcastFrequency);
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Execute() {
    /* Start from 0 */
    m_unStepCounter = 0;

    std::thread t2([&]() { m_cWebServer->Start(); });
    t2.join();
    m_cSimulationThread.join();

    // Finish all..
  }

  void CNetworkAPI::SimulationThreadFunction() {
    while (true) {
      if (
        m_eExperimentState ==
        NetworkAPI::EExperimentState::EXPERIMENT_PLAYING) {
        if (!m_cSimulator.IsExperimentFinished()) {
          /* Run user's pre step function */
          m_cSimulator.GetLoopFunctions().PreStep();

          /* Run one step */                        //
          m_cSimulator.UpdateSpace();               //
                                                    //
          /* Increment Counter */                   // 1 Step
          m_unStepCounter++;                        //
                                                    //
          /* Broadcast current experiment state */  //
          BroadcastExperimentState();               //

          /* Run user's post step function */
          m_cSimulator.GetLoopFunctions().PostStep();

          /* Take the time now */
          m_cTimer.Stop();

          /* If the elapsed time is lower than the tick length, wait */
          if (m_cTimer.Elapsed() < m_cSimulatorTickMillis) {
            /* Sleep for the difference duration */
            std::this_thread::sleep_for(
              m_cSimulatorTickMillis - m_cTimer.Elapsed());
            /* Restart Timer */
            m_cTimer.Start();
          } else {
            LOG_S(WARNING) << "Clock tick took " << m_cTimer
                           << " sec, more than the expected "
                           << m_cSimulatorTickMillis.count() << " sec. "
                           << "Recovering in next cycle." << std::endl;
            m_cTimer.Start();
          }
        } else {
          /* The experiment is done */
          m_cSimulator.GetLoopFunctions().PostExperiment();

          ResetExperiment();

          /* Change state and emit signals */
          m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
          LOG_S(INFO) << "Experiment done\n";
        }
      } else {
        /* Broadcast stopped state of experiment at 4 Hz */
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        BroadcastExperimentState();
      }
    }
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PlayExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState != NetworkAPI::EExperimentState::EXPERIMENT_PAUSED) {
      LOG_S(WARNING) << "CNetworkAPI::PlayExperiment() called in wrong state: "
                     << m_eExperimentState << std::endl;

      // silently return;
      return;
    }
    /* Change state and emit signals */
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_PLAYING;

    m_cSimulatorTickMillis = std::chrono::milliseconds(
      (long int)(CPhysicsEngine::GetSimulationClockTick() * 1000.0f));
    m_cTimer.Start();

    /* Change state and emit signals */
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_PLAYING;
    m_cWebServer->EmitEvent("Experiment playing", m_eExperimentState);

    LOG_S(INFO) << "Experiment playing";
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::PauseExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState != NetworkAPI::EExperimentState::EXPERIMENT_PLAYING) {
      LOG_S(WARNING) << "CNetworkAPI::PauseExperiment() called in wrong "
                        "state: "
                     << argos::NetworkAPI::EExperimentStateToStr(
                          m_eExperimentState);
      throw std::runtime_error(
        "Cannot pause the experiment, current state : " +
        argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState));
      return;
    }

    /* Change state and emit signals */
    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_PAUSED;
    m_cWebServer->EmitEvent("Experiment paused", m_eExperimentState);

    LOG_S(INFO) << "Experiment paused";
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::StepExperiment() {
    /* Make sure we are in the right state */
    if (
      m_eExperimentState !=
        NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED &&
      m_eExperimentState != NetworkAPI::EExperimentState::EXPERIMENT_PAUSED) {
      LOG_S(WARNING) << "CNetworkAPI::StepExperiment() called in wrong "
                        "state: "
                     << argos::NetworkAPI::EExperimentStateToStr(
                          m_eExperimentState);
      throw std::runtime_error(
        "Cannot Step the experiment, current state : " +
        argos::NetworkAPI::EExperimentStateToStr(m_eExperimentState));
      return; /* Just to be sure throw returned immediately */
    }

    if (!m_cSimulator.IsExperimentFinished()) {
      /* Run user's pre step function */
      m_cSimulator.GetLoopFunctions().PreStep();

      /* Run one step */
      m_cSimulator.UpdateSpace();

      /* Increment Counter */
      m_unStepCounter++;

      /* Run user's post step function */
      m_cSimulator.GetLoopFunctions().PostStep();

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment step done", m_eExperimentState);
    } else {
      /* The experiment is done */
      m_cSimulator.GetLoopFunctions().PostExperiment();

      ResetExperiment();

      /* Change state and emit signals */
      m_cWebServer->EmitEvent("Experiment done", m_eExperimentState);
      LOG_S(INFO) << "Experiment done\n";
    }

    /* Broadcast current experiment state */
    BroadcastExperimentState();
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::ResetExperiment() {
    /* Reset Simulator */
    m_cSimulator.Reset();

    /* Reset Counter */
    m_unStepCounter = 0;

    m_eExperimentState = NetworkAPI::EExperimentState::EXPERIMENT_INITIALIZED;

    /* Change state and emit signals */
    m_cWebServer->EmitEvent("Experiment reset", m_eExperimentState);

    /* Broadcast current experiment state */
    BroadcastExperimentState();

    LOG_S(INFO) << "Experiment reset";
  }

  /****************************************/
  /****************************************/

  void CNetworkAPI::BroadcastExperimentState() {
    /* Draw the objects */
    std::vector<nlohmann::json> vecEntitiesJson;

    CEntity::TVector& cvecEntities = m_cSpace.GetRootEntityVector();
    for (CEntity::TVector::iterator itEntities = cvecEntities.begin();
         itEntities != cvecEntities.end();
         ++itEntities) {
      nlohmann::json cEntityJson;

      /* Get the type of the entity */
      cEntityJson["type"] = (*itEntities)->GetTypeDescription();

      /* Try to get embodied entity */

      /* Is the entity embodied itself? */
      CEmbodiedEntity* pcEmbodiedEntity =
        dynamic_cast<CEmbodiedEntity*>(*itEntities);

      if (pcEmbodiedEntity == NULL) {
        /* Is the entity composable with an embodied component? */
        CComposableEntity* pcComposableTest =
          dynamic_cast<CComposableEntity*>(*itEntities);
        if (pcComposableTest != NULL) {
          if (pcComposableTest->HasComponent("body")) {
            pcEmbodiedEntity =
              &(pcComposableTest->GetComponent<CEmbodiedEntity>("body"));
          }
        }
      }

      if (pcEmbodiedEntity == NULL) {
        /* cannot find EmbodiedEntity */
        LOG_S(WARNING) << "Not an Embodied Entity :"
                       << (*itEntities)->GetTypeDescription();
        continue;
      }
      /* Get the position of the entity */
      const CVector3& cPosition = pcEmbodiedEntity->GetOriginAnchor().Position;

      /* Add it to json as=>  position:{x, y, z} */
      cEntityJson["position"]["x"] = cPosition.GetX();
      cEntityJson["position"]["y"] = cPosition.GetY();
      cEntityJson["position"]["z"] = cPosition.GetZ();

      /* Get the orientation of the entity */
      const CQuaternion& cOrientation =
        pcEmbodiedEntity->GetOriginAnchor().Orientation;

      cEntityJson["orientation"]["x"] = cOrientation.GetX();
      cEntityJson["orientation"]["y"] = cOrientation.GetY();
      cEntityJson["orientation"]["z"] = cOrientation.GetZ();
      cEntityJson["orientation"]["w"] = cOrientation.GetW();

      // m_cModel.DrawEntity(c_entity);
      vecEntitiesJson.push_back(cEntityJson);
    }
    nlohmann::json cStateJson;
    cStateJson["entities"] = vecEntitiesJson;

    /* Current state of the experiment */
    cStateJson["state"] = NetworkAPI::EExperimentStateToStr(m_eExperimentState);

    /* Current Step from the counter */
    cStateJson["steps"] = m_unStepCounter;

    /* Added Unix Epoch in milliseconds */
    cStateJson["timestamp"] =
      std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count();

    m_cWebServer->Broadcast(cStateJson);
  }

  /****************************************/
  /****************************************/

  CNetworkAPI::~CNetworkAPI() { delete m_cWebServer; }

  /****************************************/
  /****************************************/

  void CNetworkAPI::Reset() {}

  /****************************************/
  /****************************************/

  void CNetworkAPI::Destroy() {}
  /****************************************/
  /****************************************/

  REGISTER_VISUALIZATION(
    CNetworkAPI,
    "network-api",
    "Prajankya [contact@prajankya.me]",
    "1.0",
    "Network API to render over network in clientside.",
    " -- .\n",
    "It allows the user to watch and modify the "
    "simulation as it's running in an\n"
    "intuitive way.\n\n"
    "REQUIRED XML CONFIGURATION\n\n"
    "  <visualization>\n"
    "    <network-api />\n"
    "  </visualization>\n\n"
    "OPTIONAL XML CONFIGURATION\n\n");
}  // namespace argos