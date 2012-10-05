#include "NaClAMBase/NaClAMBase.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class BulletScene {
public:
  btCollisionShape* boxShape;
  btCollisionShape* groundShape;
  

  btDynamicsWorld* dynamicsWorld;
  btCollisionConfiguration* collisionConfiguration;
  btCollisionDispatcher* dispatcher;
  btBroadphaseInterface* broadphase;
  btSequentialImpulseConstraintSolver* solver;

  BulletScene() {
    dynamicsWorld = NULL;
    collisionConfiguration = NULL;
    dispatcher = NULL;
    broadphase = NULL;
    solver = NULL;
  }

  void Init() {
    boxShape = new btBoxShape(btVector3(0.50f, 0.50f, 0.50f));
    groundShape = new btStaticPlaneShape(btVector3(0.0, 1.0, 0.0), 0.0);
  }

  void EmptyScene() {
    if (dynamicsWorld) {
      int i;
      for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
          delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
      }
    }
    if (dynamicsWorld) {
      delete dynamicsWorld;
      dynamicsWorld = NULL;
    }
    if (solver) {
      delete solver;
      solver = NULL;
    }
    if (broadphase) {
      delete broadphase;
      broadphase = NULL;
    }
    if (dispatcher) {
      delete dispatcher;
      dispatcher = NULL;
    }
    if (collisionConfiguration) {
      delete collisionConfiguration;
      collisionConfiguration = NULL;
    }
  }

  void AddGroundPlane() {
    btTransform groundTransform;
    groundTransform.setIdentity();
    btScalar mass = 0.0f;
    bool isDynamic = (mass != 0.f);
    btVector3 localInertia(0,0,0);
    if (isDynamic)
      groundShape->calculateLocalInertia(mass,localInertia);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    //add the body to the dynamics world
    dynamicsWorld->addRigidBody(body);
  }

  void ResetScene() {
    EmptyScene();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new      btCollisionDispatcher(collisionConfiguration);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
                                                broadphase,
                                                solver,collisionConfiguration);
    AddGroundPlane();
  }

  

  void AddBox(const btTransform& T, float mass) {
    bool isDynamic = (mass != 0.f);
    btVector3 localInertia(0,0,0);
    if (isDynamic)
      boxShape->calculateLocalInertia(mass,localInertia);
      btDefaultMotionState* myMotionState = new btDefaultMotionState(T);
      btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,boxShape,localInertia);
      btRigidBody* body = new btRigidBody(rbInfo);
      dynamicsWorld->addRigidBody(body);
  }

  void Step() {
    if (dynamicsWorld)
      dynamicsWorld->stepSimulation(1.0/60.0);
  }
};

static BulletScene scene;

/**
 * This function is called at module initialization time.
 * moduleInterfaces and moduleInstance are already initialized.
 */
void NaClAMModuleInit() {
  NaClAMPrintf("Bullet AM Running.");
  scene.Init();
}

/**
 * This function is called at 60hz.
 * @param microseconds A monotonically increasing clock
 */
void NaClAMModuleHeartBeat(uint64_t microseconds) {

}

void handleLoadScene(const NaClAMMessage& message) {
  const palJSONObject* root = message.headerRoot;
  const palJSONObject* items = (*root)["args"];
  //const palJSONObject* items = (*args)["items"];
  scene.ResetScene();
  int children = items->GetNumChildren();
  btTransform T;
  float m[16];
  const palArray<palJSONObject*>* kids = items->GetChildArray();
  int count = 0;
  for (int i = 0; i < children; i++) {
    T.setIdentity();
    T.setOrigin(btVector3(0.0, i * 1.0, 0.0));
    palJSONObject* child = (*kids)[i];
    if (child->type == kJSONTokenTypeArray) {
      for (int j = 0; j < 16; j++) {
        palJSONObject* e = (*child)[j];
        m[j] = e->GetAsFloat();
      }
      T.setFromOpenGLMatrix(&m[0]);
      scene.AddBox(T, 1.0);
      count++;
    } else {
      NaClAMPrintf("Unknown Scene Object");
    }
  }
}

void handleStepScene(const NaClAMMessage& message) {
  if (scene.dynamicsWorld == NULL ||
      scene.dynamicsWorld->getNumCollisionObjects() == 1) {
        // No scene, just send a reply
    palDynamicString jsonMessage;
    palJSONBuilder builder;
    builder.Start(&jsonMessage);
    builder.PushObject();
    builder.Map("frames", 0, true);
    builder.Map("request", message.requestId, true);
    builder.Map("cmd", "noscene", false);
    builder.PopObject();
    PP_Var msgVar = moduleInterfaces.var->VarFromUtf8(jsonMessage.C(), jsonMessage.GetLength());
    NaClAMSendMessage(msgVar, NULL, 0);
    moduleInterfaces.var->Release(msgVar);
    return;
  }
  // Do work
  scene.Step();
  {
    // Build headers
    palDynamicString jsonMessage;
    palJSONBuilder builder;
    builder.Start(&jsonMessage);
    builder.PushObject();
    builder.Map("frames", 1, true);
    builder.Map("request", message.requestId, true);
    builder.Map("cmd", "sceneupdate", false);
    builder.PopObject();
    PP_Var msgVar = moduleInterfaces.var->VarFromUtf8(jsonMessage.C(), jsonMessage.GetLength());

    // Build transform frame
    int numObjects = scene.dynamicsWorld->getNumCollisionObjects();
    uint32_t TransformSize = (numObjects-1)*4*4*sizeof(float);
    PP_Var Transform = moduleInterfaces.varArrayBuffer->Create(TransformSize);
    float* m = (float*)moduleInterfaces.varArrayBuffer->Map(Transform);
    for (int i = 1; i < numObjects; i++) {
      btCollisionObject* obj = scene.dynamicsWorld->getCollisionObjectArray()[i];
      btRigidBody* body = btRigidBody::upcast(obj);
      if (body && body->getMotionState()) {
        btTransform xform;
        body->getMotionState()->getWorldTransform(xform);
        xform.getOpenGLMatrix(&m[0]);
      }
      m += 16;
    }
    moduleInterfaces.varArrayBuffer->Unmap(Transform);

    // Send message
    NaClAMSendMessage(msgVar, &Transform, 1);
    moduleInterfaces.var->Release(Transform);
    moduleInterfaces.var->Release(msgVar);
  }
}

/**
 * This function is called for each message received from JS
 * @param message A complete message sent from JS
 */
void NaClAMModuleHandleMessage(const NaClAMMessage& message) {
  if (message.cmdString.Equals("loadscene")) {
    handleLoadScene(message);
  } else if (message.cmdString.Equals("stepscene")) {
    handleStepScene(message);
  }
}