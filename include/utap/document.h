// -*- mode: C++; c-file-style: "stroustrup"; c-basic-offset: 4; indent-tabs-mode: nil; -*-

/* libutap - Uppaal Timed Automata Parser.
   Copyright (C) 2020 Aalborg University.
   Copyright (C) 2002-2006 Uppsala University and Aalborg University.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA
*/

#ifndef UTAP_INTERMEDIATE_HH
#define UTAP_INTERMEDIATE_HH

#include "utap/expression.h"
#include "utap/position.h"
#include "utap/symbols.h"

#include <algorithm>  // find
#include <deque>
#include <list>
#include <map>
#include <optional>
#include <vector>

namespace UTAP
{
    // Describes supported analysis methods for the given document
    struct SupportedMethods
    {
        bool symbolic{true};
        bool stochastic{true};
        bool concrete{true};
    };

    /** Base type for variables, clocks, etc.  The user data of the
        corresponding symbol_t points to this structure,
        i.e. v.uid.getData() is a pointer to v.
    */
    struct variable_t
    {
        symbol_t uid;      /**< The symbol of the variables */
        expression_t expr; /**< The initialiser */
        std::string toString() const;
    };

    /** Information about a location.
        The symbol's user data points to this structure, i.e.
        s.uid.getData() is a pointer to s. Notice that the rate list
        is generated by the type checker; until then the rate
        expressions are part of the invariant.
    */
    struct state_t
    {
        symbol_t uid;           /**< The symbol of the location */
        expression_t name;      /**< TODO: the location name with its position */
        expression_t invariant; /**< The invariant */
        expression_t exponentialRate;
        expression_t costRate; /**< Rate expression */
        int32_t locNr;         /**< Location number in template */
        std::string toString() const;
    };

    /** Information about a branchpoint.
        Branchpoints may be used in construction of edges with the
        same source, guard and synchronisation channel.
        They are not present after compilation of a model.
     */
    struct branchpoint_t
    {
        symbol_t uid;
        int32_t bpNr;
    };

    /** Information about an edge.  Edges have a source (src) and a
        destination (dst), which may be locations or branchpoints.
        The unused of these pointers should be set to nullptr.
        The guard, synchronisation and assignment are stored as
        expressions.
    */
    struct edge_t
    {
        int nr;       /**< Placement in input file */
        bool control; /**< Controllable (true/false) */
        std::string actname;
        state_t* src;        /**< Pointer to source location */
        branchpoint_t* srcb; /**< Pointer to source branchpoint */
        state_t* dst;        /**< Pointer to destination location */
        branchpoint_t* dstb; /**< Pointer to destination branchpoint */
        frame_t select;      /**< Frame for non-deterministic select */
        expression_t guard;  /**< The guard */
        expression_t assign; /**< The assignment */
        expression_t sync;   /**< The synchronisation */
        expression_t prob;   /**< Probability for probabilistic edges. */
        std::string toString() const;
        std::list<int32_t> selectValues; /**<The select values, if any */
    };

    class BlockStatement;  // Forward declaration

    /** Information about a function. The symbol's user data points to
        this structure, i.e. f.uid.getData() is a pointer to f.
    */
    struct function_t
    {
        symbol_t uid;                                  /**< The symbol of the function. */
        std::set<symbol_t> changes{};                  /**< Variables changed by this function. */
        std::set<symbol_t> depends{};                  /**< Variables the function depends on. */
        std::list<variable_t> variables{};             /**< Local variables. */
        std::unique_ptr<BlockStatement> body{nullptr}; /**< Pointer to the block. */
        function_t() = default;
        std::string toString() const;  // used to write the XML file
    };

    struct progress_t
    {
        expression_t guard;
        expression_t measure;
        progress_t(expression_t guard, expression_t measure): guard{std::move(guard)}, measure{std::move(measure)} {}
    };

    struct iodecl_t
    {
        std::string instanceName;
        std::vector<expression_t> param;
        std::list<expression_t> inputs, outputs, csp;
    };

    /**
     * Gantt map bool expr -> int expr that
     * can be expanded.
     */
    struct ganttmap_t
    {
        frame_t parameters;
        expression_t predicate, mapping;
    };

    /**
     * Gantt chart entry.
     */
    struct gantt_t
    {
        std::string name;   /**< The name */
        frame_t parameters; /**< The select parameters */
        std::list<ganttmap_t> mapping;
        explicit gantt_t(std::string name): name{std::move(name)} {}
    };

    /**
     * Structure holding declarations of various types. Used by
     * templates and block statements.
     */
    struct template_t;
    struct declarations_t
    {
        frame_t frame;
        std::list<variable_t> variables; /**< Variables */
        std::list<function_t> functions; /**< Functions */
        std::list<progress_t> progress;  /**< Progress measures */
        std::list<iodecl_t> iodecl;
        std::list<gantt_t> ganttChart;

        /** Add function declaration. */
        bool addFunction(type_t type, std::string name, position_t, function_t*&);
        /** The following methods are used to write the declarations in an XML file */
        std::string toString(bool global = false) const;
        std::string getConstants() const;
        std::string getTypeDefinitions() const;
        std::string getVariables(bool global) const;
        std::string getFunctions() const;
    };

    struct instanceLine_t;  // to be defined later

    /** Information about a message. Messages have a source (src) and a
     * destination (dst) instance lines. The label is
     * stored as an expression.
     */
    struct message_t
    {
        int nr{-1}; /**< Placement in input file */
        int location{-1};
        instanceLine_t* src{nullptr}; /**< Pointer to source instance line */
        instanceLine_t* dst{nullptr}; /**< Pointer to destination instance line */
        expression_t label;           /**< The label */
        bool isInPrechart{};
        int get_nr() const { return nr; }
        message_t() = default;
    };
    /** Information about a condition. Conditions have an anchor instance lines.
     * The label is stored as an expression.
     */
    struct condition_t
    {
        int nr{-1}; /**< Placement in input file */
        int location{-1};
        std::vector<instanceLine_t*> anchors{}; /**< Pointer to anchor instance lines */  // TODO
        expression_t label;                                                               /**< The label */
        bool isInPrechart{false};
        bool isHot{false};
        int get_nr() const { return nr; }
        condition_t() = default;
    };

    /** Information about an update. Update have an anchor instance line.
     * The label is stored as an expression.
     */
    struct update_t
    {
        int nr{-1}; /**< Placement in input file */
        int location{-1};
        instanceLine_t* anchor{nullptr}; /**< Pointer to anchor instance line */
        expression_t label;              /**< The label */
        bool isInPrechart{};
        update_t() = default;
        int get_nr() const { return nr; }
    };

    struct simregion_t
    {
        int nr{};
        message_t* message;     /** May be empty */
        condition_t* condition; /** May be empty */
        update_t* update;       /** May be empty */

        int getLoc() const;
        bool isInPrechart() const;

        simregion_t()
        {
            message = new message_t();
            condition = new condition_t();
            update = new update_t();
        }

        ~simregion_t()
        {
            delete message;
            delete condition;
            delete update;
        }

        std::string toString() const;

        void setMessage(std::deque<message_t>& messages, int nr);
        void setCondition(std::deque<condition_t>& conditions, int nr);
        void setUpdate(std::deque<update_t>& updates, int nr);
    };

    struct compare_simregion
    {
        bool operator()(const simregion_t& x, const simregion_t& y) const { return (x.getLoc() < y.getLoc()); }
    };

    struct cut_t
    {
        int nr;
        std::vector<simregion_t> simregions{};  // unordered
        explicit cut_t(int number): nr{number} {};
        void add(const simregion_t& s) { simregions.push_back(s); };
        void erase(const simregion_t& s);
        bool contains(const simregion_t& s) const;

        /**
         * returns true if the cut is in the prechart,
         * given one of the following simregions.
         * if one of the following simregions is not in the prechart,
         * then all following simregions aren't in the prechart (because of the
         * construction of the partial order),
         * and the cut is not in the prechart (but may contain only simregions
         * that are in the prechart, if it is the limit between the prechart
         * and the mainchart)
         */
        bool isInPrechart(const simregion_t& fSimregion) const;
        bool isInPrechart() const;

        bool equals(const cut_t& y) const;

        std::string toString() const
        {
            std::string s = "CUT(";
            for (unsigned int i = 0; i < simregions.size(); ++i)
                s += simregions[i].toString() + " ";
            s = s.substr(0, s.size() - 1);
            s += ")";
            return s;
        };
    };

    /**
     * Partial instance of a template. Every template is also a
     * partial instance of itself and therefore template_t is derived
     * from instance_t. A complete instance is just a partial instance
     * without any parameters.
     *
     * Even though it is possible to make partial instances of partial
     * instances, they are not represented hierarchically: All
     * parameters and arguments are merged into this one
     * struct. Therefore \a parameters contains both bound and unbound
     * symbols: Unbound symbols are parameters of this instance. Bound
     * symbols are inherited from another instance. Symbols in \a
     * parameters are ordered such that unbound symbols are listed
     * first, i.e., uid.getType().size() == parameters.getSize().
     *
     * \a mapping binds parameters to expressions.
     *
     * \a arguments is the number of arguments given by the partial
     * instance. The first \a arguments bound symbols of \a parameters
     * are the corresponding parameters. For templates, \a arguments
     * is obviously 0.
     *
     * Restricted variables are those that are used either directly or
     * indirectly in the definition of array sizes. Any restricted
     * parameters have restriction on the kind of arguments they
     * accept (they must not depend on any free process parameters).
     *
     * If i is an instance, then i.uid.getData() == i.
     */
    struct instance_t
    {
        symbol_t uid;                             /**< The name */
        frame_t parameters;                       /**< The parameters */
        std::map<symbol_t, expression_t> mapping; /**< The arguments */
        size_t arguments;
        size_t unbound;
        struct template_t* templ;
        std::set<symbol_t> restricted; /**< Restricted variables */

        std::string writeMapping() const;
        std::string writeParameters() const;
        std::string writeArguments() const;
    };

    /** Information about an instance line.
     */
    struct instanceLine_t : public instance_t
    {
        int32_t instanceNr; /**< InstanceLine number in template */
        std::vector<simregion_t> getSimregions(const std::vector<simregion_t>& simregions);
        void addParameters(instance_t& inst, frame_t params, const std::vector<expression_t>& arguments);
    };

    struct template_t : public instance_t, declarations_t
    {
        symbol_t init;                          /**< The initial location */
        frame_t templateset;                    /**< Template set decls */
        std::deque<state_t> states;             /**< Locations */
        std::deque<branchpoint_t> branchpoints; /**< Branchpoints */
        std::deque<edge_t> edges;               /**< Edges */
        std::vector<expression_t> dynamicEvals;
        bool isTA;

        int addDynamicEval(expression_t t)
        {
            dynamicEvals.push_back(t);
            return dynamicEvals.size() - 1;
        }

        std::vector<expression_t>& getDynamicEval() { return dynamicEvals; }

        /** Add another location to template. */
        state_t& addLocation(const std::string& name, expression_t inv, expression_t er, position_t pos);

        /** Add another branchpoint to template. */
        branchpoint_t& addBranchpoint(const std::string&, position_t);

        /** Add edge to template. */
        edge_t& addEdge(symbol_t src, symbol_t dst, bool type, std::string actname);

        std::deque<instanceLine_t> instances; /**< Instance Lines */
        std::deque<message_t> messages;       /**< Messages */
        std::deque<update_t> updates;         /**< Updates */
        std::deque<condition_t> conditions;   /**< Conditions */
        std::string type;
        std::string mode;
        bool hasPrechart;
        bool dynamic;
        int dynindex;
        bool isDefined;

        /** Add another instance line to template. */
        instanceLine_t& addInstanceLine();

        /** Add message to template. */
        message_t& addMessage(symbol_t src, symbol_t dst, int loc, bool pch);

        /** Add condition to template. */
        condition_t& addCondition(std::vector<symbol_t> anchors, int loc, bool pch, bool isHot);

        /** Add update to template. */
        update_t& addUpdate(symbol_t anchor, int loc, bool pch);

        bool isInvariant();  // type of the LSC

        /* gets the simregions from the LSC scenario */
        const std::vector<simregion_t> getSimregions();

        /* returns the condition on the given instance, at y location */
        bool getCondition(instanceLine_t& instance, int y, condition_t*& simCondition);

        /* returns the update on the given instance at y location */
        bool getUpdate(instanceLine_t& instance, int y, update_t*& simUpdate);

        /* returns the first update on one of the given instances, at y location */
        bool getUpdate(std::vector<instanceLine_t*>& instances, int y, update_t*& simUpdate);
    };

    /**
     * Channel priority information. Expressions must evaluate to
     * a channel or an array of channels.
     */
    struct chan_priority_t
    {
        typedef std::pair<char, expression_t> entry;
        typedef std::list<entry> tail_t;

        expression_t head;  //!< First expression in priority declaration
        tail_t tail;        //!< Pairs: separator and channel expressions

        std::string toString() const;
    };

    enum class expectation_type { Symbolic, Probability, NumericValue, _ErrorValue };

    enum class query_status_t { True, False, MaybeTrue, MaybeFalse, Unknown };

    struct option_t
    {
        std::string name;
        std::string value;
        option_t(std::string name, std::string value): name{std::move(name)}, value{std::move(value)} {}
    };

    enum class resource_type { Time, Memory };

    struct resource_t
    {
        std::string name;
        std::string value;
        std::optional<std::string> unit;
    };

    using resources_t = std::vector<resource_t>;
    using options_t = std::vector<option_t>;

    struct results_t
    {
        options_t options;  // options used for results
        std::string message;
        std::string value;  // REVISIT maybe should be variant or similar with actual value?
    };
    struct expectation_t
    {
        expectation_type value_type;
        query_status_t status;
        std::string value;
        resources_t resources;
    };

    struct query_t
    {
        std::string formula;
        std::string comment;
        options_t options;
        expectation_t expectation;
        // std::vector<results_t> results;
        std::string location;
    };
    typedef std::vector<query_t> queries_t;

    class Document;

    class SystemVisitor
    {
    public:
        virtual ~SystemVisitor() = default;
        virtual void visitSystemBefore(Document*) {}
        virtual void visitSystemAfter(Document*) {}
        virtual void visitVariable(variable_t&) {}
        virtual bool visitTemplateBefore(template_t&) { return true; }
        virtual void visitTemplateAfter(template_t&) {}
        virtual void visitState(state_t&) {}
        virtual void visitEdge(edge_t&) {}
        virtual void visitInstance(instance_t&) {}
        virtual void visitProcess(instance_t&) {}
        virtual void visitFunction(function_t&) {}
        virtual void visitTypeDef(symbol_t) {}
        virtual void visitIODecl(iodecl_t&) {}
        virtual void visitProgressMeasure(progress_t&) {}
        virtual void visitGanttChart(gantt_t&) {}
        virtual void visitInstanceLine(instanceLine_t&) {}
        virtual void visitMessage(message_t&) {}
        virtual void visitCondition(condition_t&) {}
        virtual void visitUpdate(update_t&) {}
    };

    class Document
    {
    public:
        Document();
        Document(const Document&);
        virtual ~Document() noexcept;

        /** Returns the global declarations of the document. */
        declarations_t& getGlobals();

        /** Returns the templates of the document. */
        std::list<template_t>& getTemplates();
        const template_t* findTemplate(const std::string& name) const;
        std::vector<template_t*>& getDynamicTemplates();
        template_t* getDynamicTemplate(const std::string& name);

        /** Returns the processes of the document. */
        std::list<instance_t>& getProcesses();

        options_t& getOptions();
        void setOptions(const options_t& options);

        /** Returns the queries enclosed in the model. */
        queries_t& getQueries();

        void addPosition(uint32_t position, uint32_t offset, uint32_t line, const std::string& path);
        const Positions::line_t& findPosition(uint32_t position) const;

        variable_t* addVariableToFunction(function_t*, frame_t, type_t, const std::string&, expression_t initital,
                                          position_t);
        variable_t* addVariable(declarations_t*, type_t type, const std::string&, expression_t initial, position_t);
        void addProgressMeasure(declarations_t*, expression_t guard, expression_t measure);

        template_t& addTemplate(const std::string& name, frame_t params, position_t, bool isTA = true,
                                const std::string& type = "", const std::string& mode = "");
        template_t& addDynamicTemplate(const std::string& name, frame_t params, position_t pos);

        instance_t& addInstance(const std::string& name, instance_t& instance, frame_t params,
                                const std::vector<expression_t>& arguments, position_t);

        instance_t& addLscInstance(const std::string& name, instance_t& instance, frame_t params,
                                   const std::vector<expression_t>& arguments, position_t);
        void removeProcess(instance_t& instance);  // LSC

        void copyVariablesFromTo(const template_t* from, template_t* to) const;
        void copyFunctionsFromTo(const template_t* from, template_t* to) const;

        std::string obsTA;  // name of the observer TA instance

        void addProcess(instance_t& instance, position_t);
        void addGantt(declarations_t*, gantt_t);  // copies gantt_t and moves it
        void accept(SystemVisitor&);

        void setBeforeUpdate(expression_t);
        expression_t getBeforeUpdate();
        void setAfterUpdate(expression_t);
        expression_t getAfterUpdate();

        void addQuery(query_t query);  // creates a copy and moves it
        bool queriesEmpty() const;

        /* The default priority for channels is also used for 'tau
         * transitions' (i.e. non-synchronizing transitions).
         */
        void beginChanPriority(expression_t chan);
        void addChanPriority(char separator, expression_t chan);
        const std::list<chan_priority_t>& getChanPriorities() const;
        std::list<chan_priority_t>& getMutableChanPriorities();

        /** Sets process priority for process \a name. */
        void setProcPriority(const std::string& name, int priority);

        /** Returns process priority for process \a name. */
        int getProcPriority(const char* name) const;

        /** Returns true if document has some priority declaration. */
        bool hasPriorityDeclaration() const;

        /** Returns true if document has some strict invariant. */
        bool hasStrictInvariants() const;

        /** Record that the document has some strict invariant. */
        void recordStrictInvariant();

        /** Returns true if the document stops any clock. */
        bool hasStopWatch() const;

        /** Record that the document stops a clock. */
        void recordStopWatch();

        /** Returns true if the document has guards on controllable edges with strict lower bounds. */
        bool hasStrictLowerBoundOnControllableEdges() const;

        /** Record that the document has guards on controllable edges with strict lower bounds. */
        void recordStrictLowerBoundOnControllableEdges();

        void clockGuardRecvBroadcast() { hasGuardOnRecvBroadcast = true; }
        bool hasClockGuardRecvBroadcast() const { return hasGuardOnRecvBroadcast; }
        void setSyncUsed(int s) { syncUsed = s; }
        int getSyncUsed() const { return syncUsed; }

        void setUrgentTransition() { hasUrgentTrans = true; }
        bool hasUrgentTransition() const { return hasUrgentTrans; }
        bool hasDynamicTemplates() const { return dynamicTemplates.size() != 0; }

        const std::vector<std::string>& get_strings() const { return strings; }
        void add_string(const std::string& string) { strings.push_back(string); }
        size_t add_string_if_new(const std::string& string)
        {
            auto it = std::find(std::begin(strings), std::end(strings), string);
            if (it == std::end(strings)) {
                strings.push_back(string);
                return strings.size() - 1;
            } else {
                return std::distance(std::begin(strings), it);
            }
        }

    protected:
        bool hasUrgentTrans;
        bool hasPriorities;
        bool hasStrictInv;
        bool stopsClock;
        bool hasStrictLowControlledGuards;
        bool hasGuardOnRecvBroadcast;
        int defaultChanPriority;
        std::list<chan_priority_t> chanPriorities;
        std::map<std::string, int> procPriority;
        int syncUsed;  // see typechecker

        // The list of templates.
        std::list<template_t> templates;
        // List of dynamic template
        std::list<template_t> dynamicTemplates;
        std::vector<template_t*> dynamicTemplatesVec;

        // The list of template instances.
        std::list<instance_t> instances;

        std::list<instance_t> lscInstances;
        bool modified;

        // List of processes.
        std::list<instance_t> processes;

        // Global declarations
        declarations_t global;

        expression_t beforeUpdate;
        expression_t afterUpdate;
        options_t modelOptions;
        queries_t queries;

        variable_t* addVariable(std::list<variable_t>& variables, frame_t frame, type_t type, const std::string&,
                                position_t);

        std::string location;
        std::vector<void*> libraries;
        std::vector<std::string> strings;
        SupportedMethods supportedMethods{};

    public:
        void addLibrary(void* lib);
        void* lastLibrary();
        void addError(position_t, std::string msg, std::string ctx = "");
        void addWarning(position_t, const std::string& msg, const std::string& ctx = "");
        bool hasErrors() const { return !errors.empty(); }
        bool hasWarnings() const { return !warnings.empty(); }
        const std::vector<error_t>& getErrors() const { return errors; }
        const std::vector<error_t>& getWarnings() const { return warnings; }
        void clearErrors() const;
        void clearWarnings() const;
        bool isModified() const;
        void setModified(bool mod);
        iodecl_t* addIODecl();
        void setSupportedMethods(const SupportedMethods& supportedMethods);
        const SupportedMethods& getSupportedMethods() const;

    private:
        // TODO: move errors & warnings to ParserBuilder to get rid of mutable
        mutable std::vector<error_t> errors;
        mutable std::vector<error_t> warnings;
        Positions positions;
    };
}  // namespace UTAP

#endif /* UTAP_INTERMEDIATE_HH */
