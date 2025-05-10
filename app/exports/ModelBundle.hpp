#ifndef MODEL_BUNDLE_SOL_HPP_
#define MODEL_BUNDLE_SOL_HPP_
#include <vector>
#include <limits>
#include <ModelContainer.hpp>

// Should contain all the models of a Model Bundle which have a certain pipeline.
class PipelineModelBundle
{
public:
	PipelineModelBundle() : m_modelIndicesInBundle{}, m_pipelineIndex{ 0u } {}

	void SetPipelineIndex(std::uint32_t index) noexcept { m_pipelineIndex = index; }

	void AddModelIndex(std::uint32_t indexInBundle) noexcept
	{
		m_modelIndicesInBundle.emplace_back(indexInBundle);
	}

	void RemoveModelIndex(std::uint32_t indexInBundle) noexcept
	{
		std::erase(m_modelIndicesInBundle, indexInBundle);
	}

	[[nodiscard]]
	std::uint32_t GetPipelineIndex() const noexcept { return m_pipelineIndex; }
	[[nodiscard]]
	const std::vector<std::uint32_t>& GetModelIndicesInBundle() const noexcept
	{
		return m_modelIndicesInBundle;
	}
	[[nodiscard]]
	size_t GetModelCount() const noexcept { return std::size(m_modelIndicesInBundle); }

private:
	std::vector<std::uint32_t> m_modelIndicesInBundle;
	std::uint32_t              m_pipelineIndex;

public:
	PipelineModelBundle(const PipelineModelBundle&) = delete;
	PipelineModelBundle& operator=(const PipelineModelBundle&) = delete;

	PipelineModelBundle(PipelineModelBundle&& other) noexcept
		: m_modelIndicesInBundle{ std::move(other.m_modelIndicesInBundle) },
		m_pipelineIndex{ other.m_pipelineIndex }
	{}
	PipelineModelBundle& operator=(PipelineModelBundle&& other) noexcept
	{
		m_modelIndicesInBundle = std::move(other.m_modelIndicesInBundle);
		m_pipelineIndex        = other.m_pipelineIndex;

		return *this;
	}
};

// Should typically have a complex model with multiple models.
class ModelBundle
{
	using PipelineContainer_t = std::vector<PipelineModelBundle>;

public:
	ModelBundle()
		: m_modelContainer{}, m_modelIndicesInContainer{}, m_pipelines{}, m_meshBundleIndex{ 0u }
	{}

	void SetMeshBundleIndex(std::uint32_t index) noexcept { m_meshBundleIndex = index; }

	void SetModelContainer(std::shared_ptr<ModelContainer> modelContainer) noexcept
	{
		m_modelContainer = std::move(modelContainer);
	}

	void SetModelPipeline(size_t localIndex, std::uint32_t pipelineIndex) noexcept
	{
		const size_t pipelineLocalIndex = GetOrAddPipeline(pipelineIndex);

		m_pipelines[pipelineLocalIndex].AddModelIndex(static_cast<std::uint32_t>(localIndex));
	}

	std::uint32_t AddModel(Model&& model) noexcept
	{
		const auto localIndex = static_cast<std::uint32_t>(std::size(m_modelIndicesInContainer));

		m_modelIndicesInContainer.emplace_back(m_modelContainer->AddModel(std::move(model)));

		return localIndex;
	}

	std::uint32_t AddModel(Model&& model, std::uint32_t pipelineIndex) noexcept
	{
		const std::uint32_t localIndex = AddModel(std::move(model));

		SetModelPipeline(localIndex, pipelineIndex);

		return localIndex;
	}

	size_t AddPipeline(PipelineModelBundle&& pipeline) noexcept
	{
		const size_t localIndex = std::size(m_pipelines);

		m_pipelines.emplace_back(std::move(pipeline));

		return localIndex;
	}

	[[nodiscard]]
	size_t AddPipeline(std::uint32_t pipelineIndex) noexcept
	{
		PipelineModelBundle pipeline{};

		pipeline.SetPipelineIndex(pipelineIndex);

		return AddPipeline(std::move(pipeline));
	}

	[[nodiscard]]
	std::optional<size_t> FindLocalPipelineIndex(std::uint32_t pipelineIndex) const noexcept
	{
		std::optional<size_t> oLocalIndex{};

		const size_t pipelineCount = GetPipelineCount();

		size_t pipelineLocalIndex  = std::numeric_limits<size_t>::max();

		// Can replace this search with an unordered_map but I don't think there will be too
		// many pipelines, so gonna keep it a linear search for now.
		for (size_t index = 0u; index < pipelineCount; ++index)
			if (m_pipelines[index].GetPipelineIndex() == pipelineIndex)
			{
				pipelineLocalIndex = index;

				break;
			}

		if (pipelineLocalIndex != std::numeric_limits<size_t>::max())
			oLocalIndex = pipelineLocalIndex;

		return oLocalIndex;
	}

	void ChangeModelPipeline(
		std::uint32_t modelIndexInBundle, std::uint32_t oldPipelineIndex,
		std::uint32_t newPipelineIndex
	) noexcept {
		size_t oldPipelineIndexInBundle = std::numeric_limits<size_t>::max();
		size_t newPipelineIndexInBundle = std::numeric_limits<size_t>::max();

		const size_t pipelineCount = GetPipelineCount();

		for (size_t index = 0u; index < pipelineCount; ++index)
		{
			const size_t currentPipelineIndex = m_pipelines[index].GetPipelineIndex();

			if (currentPipelineIndex == oldPipelineIndex)
				oldPipelineIndexInBundle = index;

			if (currentPipelineIndex == newPipelineIndex)
				newPipelineIndexInBundle = index;

			const bool bothFound = oldPipelineIndexInBundle != std::numeric_limits<size_t>::max()
				&& newPipelineIndexInBundle != std::numeric_limits<size_t>::max();

			if (bothFound)
				break;
		}

		m_pipelines[oldPipelineIndexInBundle].RemoveModelIndex(modelIndexInBundle);
		m_pipelines[newPipelineIndexInBundle].AddModelIndex(modelIndexInBundle);
	}

	[[nodiscard]]
	std::uint32_t GetMeshBundleIndex() const noexcept { return m_meshBundleIndex; }

	[[nodiscard]]
	const PipelineModelBundle& GetPipeline(size_t localIndex) const noexcept
	{
		return m_pipelines[localIndex];
	}

	[[nodiscard]]
	const std::vector<PipelineModelBundle>& GetPipelines() const noexcept
	{
		return m_pipelines;
	}

	[[nodiscard]]
	auto&& GetModel(this auto&& self, size_t localIndex) noexcept
	{
		const size_t containerIndex = self.m_modelIndicesInContainer[localIndex];

		return std::forward_like<decltype(self)>(self.m_modelContainer->GetModel(containerIndex));
	}

	[[nodiscard]]
	std::uint32_t GetIndexInContainer(size_t localIndex) noexcept
	{
		return m_modelIndicesInContainer[localIndex];
	}

	[[nodiscard]]
	const std::vector<std::uint32_t>& GetIndicesInContainer() const noexcept
	{
		return m_modelIndicesInContainer;
	}

	[[nodiscard]]
	const std::shared_ptr<ModelContainer>& GetModelContainer() const noexcept
	{
		return m_modelContainer;
	}

	[[nodiscard]]
	size_t GetModelCount() const noexcept { return std::size(m_modelIndicesInContainer); }
	[[nodiscard]]
	size_t GetPipelineCount() const noexcept { return std::size(m_pipelines); }

private:
	[[nodiscard]]
	size_t GetOrAddPipeline(std::uint32_t pipelineIndex) noexcept
	{
		size_t pipelineLocalIndex  = std::numeric_limits<size_t>::max();

		std::optional<size_t> oPipelineLocalIndex = FindLocalPipelineIndex(
			pipelineIndex
		);

		if (!oPipelineLocalIndex)
			pipelineLocalIndex = AddPipeline(pipelineIndex);
		else
			pipelineLocalIndex = *oPipelineLocalIndex;

		return pipelineLocalIndex;
	}

private:
	std::shared_ptr<ModelContainer> m_modelContainer;
	std::vector<std::uint32_t>      m_modelIndicesInContainer;
	PipelineContainer_t             m_pipelines;
	std::uint32_t                   m_meshBundleIndex;

public:
	ModelBundle(const ModelBundle&) = delete;
	ModelBundle& operator=(const ModelBundle&) = delete;

	ModelBundle(ModelBundle&& other) noexcept
		: m_modelContainer{ std::move(other.m_modelContainer) },
		m_pipelines{ std::move(other.m_pipelines) },
		m_meshBundleIndex{ other.m_meshBundleIndex }
	{}
	ModelBundle& operator=(ModelBundle&& other) noexcept
	{
		m_modelContainer  = std::move(other.m_modelContainer);
		m_pipelines       = std::move(other.m_pipelines);
		m_meshBundleIndex = other.m_meshBundleIndex;

		return *this;
	}
};

#endif
