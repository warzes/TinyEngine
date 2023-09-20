#pragma once

#if USE_PHYSICS

// An element which can belong to an island.
class IslandElement
{
public:
	IslandElement() = default;
	virtual ~IslandElement() = default;

	void SetIslandElementId(unsigned int islandElementId) { m_islandElementId = islandElementId; }
	unsigned int GetIslandElementId() const { return m_islandElementId; }

	virtual bool IsActive() const = 0;

private:
	unsigned int m_islandElementId = 0;
};

// Allow to make the link between an island element and the island ID.
struct IslandElementLink final
{
	IslandElement* element = nullptr;   // reference to the element
	unsigned int islandIdRef = 0;       // reference to the next island element. If it references itself: it's the island id.
	bool linkedToStaticElement = false; // true if 'element' is linked to a static element.
};

/**
* Island container. An island is a set of elements/bodies which are in contact.
* The island are useful to made sleep elements/bodies. If all elements/bodies of an island doesn't
* move: there is no need to execute collision detection checks on these island elements/bodies.
*/
class IslandContainer final
{
public:
	// Reset the container of island elements. Create islands of one element for each island elements asked.
	void Reset(const std::vector<IslandElement*>& islandElements);
	void MergeIsland(const IslandElement& element1, const IslandElement& element2);
	void LinkToStaticElement(const IslandElement& element);

	// Sorts the islands by ID and returns them. Once the islands sorted, the container is not usable anymore and need to be reset.
	const std::vector<IslandElementLink>& RetrieveSortedIslandElements();

private:
	unsigned int findIslandId(unsigned int elementRef) const;

	std::vector<IslandElementLink> m_islandElementsLink;
	bool m_containerSorted = false;
};

#endif // USE_PHYSICS