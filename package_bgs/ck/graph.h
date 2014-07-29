/* graph.h */
/*
  This software library implements the maxflow algorithm
  described in

  An Experimental Comparison of Min-Cut/Max-Flow Algorithms
  for Energy Minimization in Vision.
  Yuri Boykov and Vladimir Kolmogorov.
  In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI),
  September 2004

  This algorithm was developed by Yuri Boykov and Vladimir Kolmogorov
  at Siemens Corporate Research. To make it available for public use,
  it was later reimplemented by Vladimir Kolmogorov based on open publications.

  If you use this software for research purposes, you should cite
  the aforementioned paper in any resulting publication.
  */

/*
  Copyright 2001 Vladimir Kolmogorov (vnk@cs.cornell.edu), Yuri Boykov (yuri@csd.uwo.ca).

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  */


/*
  For description, example usage, discussion of graph representation
  and memory usage see README.TXT.
  */

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "block.h"

/*
  Nodes, arcs and pointers to nodes are
  added in blocks for memory and time efficiency.
  Below are numbers of items in blocks
  */
#define NODE_BLOCK_SIZE 512
#define ARC_BLOCK_SIZE 1024
#define NODEPTR_BLOCK_SIZE 128

namespace ck
{
  class Graph
  {
  public:
    typedef enum
    {
      SOURCE = 0,
      SINK = 1
    } termtype; /* terminals */

    /* Type of edge weights.
       Can be changed to char, int, float, double, ... */
    typedef short captype;
    /* Type of total flow */
    typedef int flowtype;

    typedef void * node_id;

    /* interface functions */

    /* Constructor. Optional argument is the pointer to the
       function which will be called if an error occurs;
       an error message is passed to this function. If this
       argument is omitted, exit(1) will be called. */
    Graph(void(*err_function)(char *) = NULL);

    /* Destructor */
    ~Graph();

    /* Adds a node to the graph */
    node_id add_node();

    /* Adds a bidirectional edge between 'from' and 'to'
       with the weights 'cap' and 'rev_cap' */
    void add_edge(node_id from, node_id to, captype cap, captype rev_cap);

    /* Sets the weights of the edges 'SOURCE->i' and 'i->SINK'
       Can be called at most once for each node before any call to 'add_tweights'.
       Weights can be negative */
    void set_tweights(node_id i, captype cap_source, captype cap_sink);

    /* Adds new edges 'SOURCE->i' and 'i->SINK' with corresponding weights
       Can be called multiple times for each node.
       Weights can be negative */
    void add_tweights(node_id i, captype cap_source, captype cap_sink);

    /* After the maxflow is computed, this function returns to which
       segment the node 'i' belongs (Graph::SOURCE or Graph::SINK) */
    termtype what_segment(node_id i);

    /* Computes the maxflow. Can be called only once. */
    flowtype maxflow();

    /***********************************************************************/
    /***********************************************************************/
    /***********************************************************************/

  private:
    /* internal variables and functions */

    struct arc_st;

    /* node structure */
    typedef struct node_st
    {
      arc_st			*first;		/* first outcoming arc */

      arc_st			*parent;	/* node's parent */
      node_st			*next;		/* pointer to the next active node
                       (or to itself if it is the last node in the list) */
      int				TS;			/* timestamp showing when DIST was computed */
      int				DIST;		/* distance to the terminal */
      short			is_sink;	/* flag showing whether the node is in the source or in the sink tree */

      captype			tr_cap;		/* if tr_cap > 0 then tr_cap is residual capacity of the arc SOURCE->node
                       otherwise         -tr_cap is residual capacity of the arc node->SINK */
    } node;

    /* arc structure */
    typedef struct arc_st
    {
      node_st			*head;		/* node the arc points to */
      arc_st			*next;		/* next arc with the same originating node */
      arc_st			*sister;	/* reverse arc */

      captype			r_cap;		/* residual capacity */
    } arc;

    /* 'pointer to node' structure */
    typedef struct nodeptr_st
    {
      node_st			*ptr;
      nodeptr_st		*next;
    } nodeptr;

    Block<node>			*node_block;
    Block<arc>			*arc_block;
    DBlock<nodeptr>		*nodeptr_block;

    void(*error_function)(char *);	/* this function is called if a error occurs,
                         with a corresponding error message
                         (or exit(1) is called if it's NULL) */

    flowtype			flow;		/* total flow */

    /***********************************************************************/

    node				*queue_first[2], *queue_last[2];	/* list of active nodes */
    nodeptr				*orphan_first, *orphan_last;		/* list of pointers to orphans */
    int					TIME;								/* monotonically increasing global counter */

    /***********************************************************************/

    /* functions for processing active list */
    void set_active(node *i);
    node *next_active();

    void maxflow_init();
    void augment(arc *middle_arc);
    void process_source_orphan(node *i);
    void process_sink_orphan(node *i);
  };
}

#endif
