#ifndef CHIG_NODE_TYPE_HPP
#define CHIG_NODE_TYPE_HPP

#pragma once

#include "chig/Context.hpp"
#include "chig/DataType.hpp"
#include "chig/Result.hpp"
#include "chig/json.hpp"

#include <iterator>
#include <utility>

#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

namespace chig
{
/// A generic node type. All user made types are of JsonNodeType type, which is defined in
/// JsonModule.cpp. This allows for easy extension of the language.
struct NodeType {
	friend class NodeInstance;

	/// Constructor
	/// \param mod The module to create the NodeType in
	/// \param name The name of the NodeType
	/// \param description The description of the NodeType
	NodeType(ChigModule& mod, gsl::cstring_span<> name, gsl::cstring_span<> description)
		: mModule{&mod},
		  mContext{&mod.context()},
		  mName{gsl::to_string(name)},
		  mDescription{gsl::to_string(description)}
	{
	}
	/// Destructor
	virtual ~NodeType() = default;

	/// Get the qualified name of the node type, like module.name():name()
	/// \return The qualified name
	std::string qualifiedName() const { return module().name() + ":" + name(); }
	/// A virtual function that is called when this node needs to be called
	/// \param execInputID The ID of the exec input
	/// \param modToCodegenInto The module that is being generated
	/// \param dBuilder Debug info builder
	/// \param f The function that is being generated
	/// \param diFunc The Debug function
	/// \param io This has the values that are the inputs and outputs of the function.
	/// This vector will always have the size of `inputs.size() + outputs.size()` and starts with
	/// the inputs.
	/// The types are gaurenteed to be the same as inputs and outputs
	/// \param codegenInto The IRBuilder object that is used to place calls into
	/// \param outputBlocks The blocks that can be outputted. This will be the same size as
	/// numOutputExecs.
	/// \return The Result
	virtual Result codegen(size_t execInputID, llvm::Module* modToCodegenInto,
		llvm::DIBuilder* dBuilder, llvm::Function* f, llvm::DISubprogram* diFunc,
		const gsl::span<llvm::Value*> io, llvm::BasicBlock* codegenInto,
		const gsl::span<llvm::BasicBlock*> outputBlocks) const = 0;

	/// Create the JSON necessary to store the object.
	/// \return The json obejct
	virtual nlohmann::json toJSON() const { return {}; }
	/// Clones the type
	/// \return The clone
	virtual std::unique_ptr<NodeType> clone() const = 0;

	/// Get the name of the NodeType in the ChigModule.
	/// \return The name
	std::string name() const { return mName; }
	/// Get the description of the NodeType
	/// \return The description
	std::string description() const { return mDescription; }
	/// Get the ChigModule this NodeType belongs to
	/// \return The ChigModule
	ChigModule& module() const { return *mModule; }
	/// Get the Context this NodeType belongs to
	/// \return The Context
	Context& context() const { return *mContext; }
	/// Get the data inputs for the node
	/// \return The data inputs in the format of {{DataType, description}, ...}
	gsl::span<const std::pair<DataType, std::string>> dataInputs() const { return mDataInputs; }
	/// Get the data outputs for the node
	/// \return The data outputs in the format of {{DataType, description}, ...}
	gsl::span<const std::pair<DataType, std::string>> dataOutputs() const { return mDataOutputs; }
	/// Get the execution inputs for the node
	/// \return The names of the inputs. The size of this vector is the size of inputs.
	gsl::span<const std::string> execInputs() const { return mExecInputs; }
	/// Get the execution outputs for the node
	/// \return The names of the outputs. The size is the input count.
	gsl::span<const std::string> execOutputs() const { return mExecOutputs; }
protected:
	/// Set the data inputs for the NodeType
	/// \param newInputs The new inputs
	void setDataInputs(std::vector<std::pair<DataType, std::string>> newInputs)
	{
		mDataInputs = std::move(newInputs);
	}
	/// Set the data outputs for the NodeType
	/// \param newOutputs The new outputs
	void setDataOutputs(std::vector<std::pair<DataType, std::string>> newOutputs)
	{
		mDataOutputs = std::move(newOutputs);
	}
	/// Set the exec inputs for the NodeType
	/// \param newInputs The new inputs
	void setExecInputs(std::vector<std::string> newInputs) { mExecInputs = std::move(newInputs); }
	/// Set the exec outputs for the NodeType
	/// \param newOutputs The new outputs
	void setExecOutputs(std::vector<std::string> newOutputs)
	{
		mExecOutputs = std::move(newOutputs);
	}

	/// Get the node instance
	/// \return the node instance
	NodeInstance* nodeInstance() const { return mNodeInstance; }
private:
	ChigModule* mModule;
	Context* mContext;
	std::string mName, mDescription;

	NodeInstance* mNodeInstance = nullptr;

	std::vector<std::pair<DataType, std::string>> mDataInputs;
	std::vector<std::pair<DataType, std::string>> mDataOutputs;

	std::vector<std::string> mExecInputs;
	std::vector<std::string> mExecOutputs;
};
}

#endif  // CHIG_NODE_TYPE_HPP
