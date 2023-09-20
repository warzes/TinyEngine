#include "stdafx.h"
#if USE_PHYSICS
#include "IslandElement.h"
//-----------------------------------------------------------------------------
void IslandContainer::Reset(const std::vector<IslandElement*>& islandElements)
{
	m_containerSorted = false;

	m_islandElementsLink.clear();
	m_islandElementsLink.resize(islandElements.size());

	for (std::size_t i = 0; i < islandElements.size(); ++i) 
	{
		islandElements[i]->SetIslandElementId(static_cast<unsigned int>(i));

		m_islandElementsLink[i].element = islandElements[i];
		m_islandElementsLink[i].linkedToStaticElement = !islandElements[i]->IsActive();
		m_islandElementsLink[i].islandIdRef = static_cast<unsigned int>(i);
	}
}
//-----------------------------------------------------------------------------
void IslandContainer::MergeIsland(const IslandElement& element1, const IslandElement& element2)
{
	assert(!m_containerSorted);

	const unsigned int islandId1 = findIslandId(element1.GetIslandElementId());
	const unsigned int islandId2 = findIslandId(element2.GetIslandElementId());

	if (islandId1 == islandId2) // elements are already in the same island
		return;

	m_islandElementsLink[islandId1].islandIdRef = islandId2;
}
//-----------------------------------------------------------------------------
void IslandContainer::LinkToStaticElement(const IslandElement& element)
{
	const unsigned int islandId = findIslandId(element.GetIslandElementId());
	m_islandElementsLink[islandId].linkedToStaticElement = true;
}
//-----------------------------------------------------------------------------
const std::vector<IslandElementLink>& IslandContainer::RetrieveSortedIslandElements()
{
	//store directly island ID on islandIdRef instead of reference
	for (std::size_t i = 0; i < m_islandElementsLink.size(); ++i) 
		m_islandElementsLink[i].islandIdRef = findIslandId((unsigned int)i);

	std::ranges::sort(m_islandElementsLink, [](const auto& lhs, const auto& rhs) {return lhs.islandIdRef < rhs.islandIdRef; });
	m_containerSorted = true;

	return m_islandElementsLink;
}
//-----------------------------------------------------------------------------
unsigned int IslandContainer::findIslandId(unsigned int elementRef) const
{
	while (elementRef != m_islandElementsLink[elementRef].islandIdRef)
		elementRef = m_islandElementsLink[elementRef].islandIdRef;

	return elementRef;
}
//-----------------------------------------------------------------------------
#endif // USE_PHYSICS