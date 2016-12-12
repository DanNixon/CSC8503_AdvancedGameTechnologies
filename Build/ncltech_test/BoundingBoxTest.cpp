#include <CppUnitTest.h>

#include <ncltech/BoundingBox.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off
TEST_CLASS(BoundingBoxTest)
{
public:
  TEST_METHOD(BoundingBox_HalfDim)
  {
    BoundingBox b;
    b.SetHalfDimensions(Vector3(10.0f, 12.0f, 18.0f));

    Assert::IsTrue(b.Lower() == Vector3(-10.0f, -12.0f, -18.0f));
    Assert::IsTrue(b.Upper() == Vector3(10.0f, 12.0f, 18.0f));
  }

  TEST_METHOD(BoundingBox_ExpandByPoint)
  {
    BoundingBox b;
    
    b.ExpandToFit(Vector3(1.0f, 2.0f, 3.0f));
    Assert::IsTrue(b.Lower() == Vector3(1.0f, 2.0f, 3.0f));
    Assert::IsTrue(b.Upper() == Vector3(1.0f, 2.0f, 3.0f));

    b.ExpandToFit(Vector3(1.0f, -7.0f, 3.0f));
    Assert::IsTrue(b.Lower() == Vector3(1.0f, -7.0f, 3.0f));
    Assert::IsTrue(b.Upper() == Vector3(1.0f, 2.0f, 3.0f));

    b.ExpandToFit(Vector3(-10.0f, -7.0f, 30.0f));
    Assert::IsTrue(b.Lower() == Vector3(-10.0f, -7.0f, 3.0f));
    Assert::IsTrue(b.Upper() == Vector3(1.0f, 2.0f, 30.0f));
  }

  TEST_METHOD(BoundingBox_ExpandByBox)
  {
    BoundingBox b;
    
    b.ExpandToFit(BoundingBox(Vector3(-2.0f, -1.0f, 0.0f), Vector3(9.0f, 3.0f, 10.0f)));
    Assert::IsTrue(b.Lower() == Vector3(-2.0f, -1.0f, 0.0f));
    Assert::IsTrue(b.Upper() == Vector3(9.0f, 3.0f, 10.0f));
    
    b.ExpandToFit(BoundingBox(Vector3(-7.0f, -1.0f, 5.0f), Vector3(12.0f, 3.0f, 10.0f)));
    Assert::IsTrue(b.Lower() == Vector3(-7.0f, -1.0f, 0.0f));
    Assert::IsTrue(b.Upper() == Vector3(12.0f, 3.0f, 10.0f));
  }

  TEST_METHOD(BoundingBox_Centre)
  {
    BoundingBox b(Vector3(-6.0f, 18.0f, 25.0f), Vector3(-4.0f, 21.0f, 67.0f));
    Assert::IsTrue(b.Centre() == Vector3(-5.0f, 19.5f, 46.0f));
  }

  TEST_METHOD(BoundingBox_PointIntersects)
  {
    BoundingBox b(Vector3(-1.0f, -2.0f, -3.0f), Vector3(7.0f, 8.0f, 9.0f));

    Assert::IsTrue(b.Intersects(Vector3(0.0f, 0.0f, 0.0f)));
    Assert::IsFalse(b.Intersects(Vector3(-9.0f, 0.0f, 0.0f)));
    Assert::IsTrue(b.Intersects(Vector3(7.0f, 8.0f, 9.0f)));
    Assert::IsTrue(b.Intersects(Vector3(-1.0f, -2.0f, -3.0f)));
  }

  TEST_METHOD(BoundingBox_BoxIntersects)
  {
    BoundingBox b1(Vector3(-1.0f, -2.0f, -3.0f), Vector3(7.0f, 8.0f, 9.0f));
    BoundingBox b2(Vector3(-9.0f, -1.0f, -2.0f), Vector3(0.0f, 6.0f, 8.0f));
    BoundingBox b3(Vector3(1.0f, 0.0f, 0.0f), Vector3(9.0f, 0.0f, 0.0f));

    Assert::IsTrue(b1.Intersects(b2));
    Assert::IsTrue(b1.Intersects(b3));
    Assert::IsFalse(b2.Intersects(b3));
  }

  TEST_METHOD(BoundingBox_BoundingSphereRadius)
  {
    {
      BoundingBox b;
      b.SetHalfDimensions(Vector3(10.0f, 12.0f, 18.0f));
      Assert::AreEqual(18.0f, b.SphereRadius());
    }

    {
      BoundingBox b;
      b.SetHalfDimensions(Vector3(12.0f, 10.0f, 6.0f));
      Assert::AreEqual(12.0f, b.SphereRadius());
    }

    {
      BoundingBox b;
      b.SetHalfDimensions(Vector3(100.0f, 120.0f, 18.0f));
      Assert::AreEqual(120.0f, b.SphereRadius());
    }
  }
};