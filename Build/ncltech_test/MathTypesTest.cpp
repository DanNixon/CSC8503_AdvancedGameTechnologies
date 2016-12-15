#include <CppUnitTest.h>

#include <nclgl/Vector2.h>
#include <nclgl/Vector3.h>
#include <nclgl/Vector4.h>
#include <nclgl/Quaternion.h>

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(MathTypesTest)
{
public:
  TEST_METHOD(Vector2_operatorStreamOutput)
  {
    Vector2 v(5.1f, 3.2f);
    std::stringstream stream;
    stream << v;
    Assert::AreEqual(std::string("[5.1,3.2]"), stream.str());
  }

  TEST_METHOD(Vector2_operatorStreamInput)
  {
    std::stringstream stream;
    stream << "[5.1,3.2]";

    Vector2 v;
    stream >> v;

    Assert::AreEqual(5.1f, v.x);
    Assert::AreEqual(3.2f, v.y);
  }

  TEST_METHOD(Vector3_operatorStreamOutput)
  {
    Vector3 v(5.1f, 3.2f, 2.3f);
    std::stringstream stream;
    stream << v;
    Assert::AreEqual(std::string("[5.1,3.2,2.3]"), stream.str());
  }

  TEST_METHOD(Vector3_operatorStreamInput)
  {
    std::stringstream stream;
    stream << "[5.1,3.2,2.3]";

    Vector3 v;
    stream >> v;

    Assert::AreEqual(5.1f, v.x);
    Assert::AreEqual(3.2f, v.y);
    Assert::AreEqual(2.3f, v.z);
  }

  TEST_METHOD(Vector4_operatorStreamOutput)
  {
    Vector4 v(5.1f, 3.2f, 2.3f, 8.4f);
    std::stringstream stream;
    stream << v;
    Assert::AreEqual(std::string("[5.1,3.2,2.3,8.4]"), stream.str());
  }

  TEST_METHOD(Vector4_operatorStreamInput)
  {
    std::stringstream stream;
    stream << "[5.1,3.2,2.3,8.4]";

    Vector4 v;
    stream >> v;

    Assert::AreEqual(5.1f, v.x);
    Assert::AreEqual(3.2f, v.y);
    Assert::AreEqual(2.3f, v.z);
    Assert::AreEqual(8.4f, v.w);
  }

  TEST_METHOD(Quaternion_StreamOutput)
  {
    Quaternion q(5.0f, 2.0f, 4.5f, 8.9f);
    std::stringstream stream;
    stream << q;
    Assert::AreEqual(stream.str(), std::string("[5,2,4.5,8.9]"));
  }

  TEST_METHOD(Quaternion_StreamInput)
  {
    /* Create stream with valid string */
    std::stringstream stream;
    stream << "[5.0,2.0,4.5,8.9]";

    /* Read the stream */
    Quaternion q;
    stream >> q;

    /* Validate results */
    Assert::AreEqual(5.0f, q.x);
    Assert::AreEqual(2.0f, q.y);
    Assert::AreEqual(4.5f, q.z);
    Assert::AreEqual(8.9f, q.w);
  }
};