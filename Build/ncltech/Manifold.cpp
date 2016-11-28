#include "Manifold.h"
#include <nclgl\Matrix3.h>
#include "NCLDebug.h"
#include "PhysicsEngine.h"

#define persistentThresholdSq 0.025f

typedef std::list<ContactPoint> ContactList;
typedef ContactList::iterator ContactListItr;

Manifold::Manifold() 
	: m_pNodeA(NULL)
	, m_pNodeB(NULL)
{
}

Manifold::~Manifold()
{

}

void Manifold::Initiate(PhysicsObject* nodeA, PhysicsObject* nodeB)
{
	m_vContacts.clear();

	m_pNodeA = nodeA;
	m_pNodeB = nodeB;
}

void Manifold::ApplyImpulse()
{
	/* TUT 6 CODE HERE */
}


void Manifold::SolveContactPoint(ContactPoint& c)
{
	/* TUT 6 CODE HERE */
}

void Manifold::PreSolverStep(float dt)
{
	for (ContactPoint& contact : m_vContacts)
	{
		UpdateConstraint(contact);
	}
}

void Manifold::UpdateConstraint(ContactPoint& contact)
{
	//Reset total impulse forces computed this physics timestep 
	contact.sumImpulseContact = 0.0f;
	contact.sumImpulseFriction = Vector3(0.0f, 0.0f, 0.0f);


	/* TUT 6 CODE HERE */
}

void Manifold::AddContact(const Vector3& globalOnA, const Vector3& globalOnB, const Vector3& _normal, const float& _penetration)
{
	//Get relative offsets from each object centre of mass
	// Used to compute rotational velocity at the point of contact.
	Vector3 r1 = (globalOnA - m_pNodeA->GetPosition());
	Vector3 r2 = (globalOnB - m_pNodeB->GetPosition());

	//Create our new contact descriptor
	ContactPoint contact;
	contact.relPosA = r1;
	contact.relPosB = r2;
	contact.collisionNormal = _normal;
	contact.collisionPenetration = _penetration;


	//Check to see if we already contain a contact point almost in that location
	const float min_allowed_dist_sq = 0.2f * 0.2f;
	bool should_add = true;
	for (auto itr = m_vContacts.begin(); itr != m_vContacts.end(); )
	{
		Vector3 ab = itr->relPosA - contact.relPosA;
		float distsq = Vector3::Dot(ab, ab);


		//Choose the contact point with the largest penetration and therefore the largest collision response
		if (distsq < min_allowed_dist_sq)
		{
			if (itr->collisionPenetration > contact.collisionPenetration)
			{
				itr = m_vContacts.erase(itr);
				continue;
			}
			else
			{
				should_add = false;
			}
			
		}
		
		itr++;
	}


	
	if (should_add)
		m_vContacts.push_back(contact);
}

void Manifold::DebugDraw() const
{
	if (m_vContacts.size() > 0)
	{
		//Loop around all contact points and draw them all as a line-fan
		Vector3 globalOnA1 = m_pNodeA->GetPosition() + m_vContacts.back().relPosA;
		for (const ContactPoint& contact : m_vContacts)
		{
			Vector3 globalOnA2 = m_pNodeA->GetPosition() + contact.relPosA;
			Vector3 globalOnB = m_pNodeB->GetPosition() + contact.relPosB;

			//Draw line to form area given by all contact points
			NCLDebug::DrawThickLineNDT(globalOnA1, globalOnA2, 0.02f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

			//Draw descriptors for indivdual contact point
			NCLDebug::DrawPointNDT(globalOnA2, 0.05f, Vector4(0.0f, 0.5f, 0.0f, 1.0f));
			NCLDebug::DrawThickLineNDT(globalOnB, globalOnA2, 0.01f, Vector4(1.0f, 0.0f, 1.0f, 1.0f));

			globalOnA1 = globalOnA2;
		}
	}
}