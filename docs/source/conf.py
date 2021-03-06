# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

import subprocess, os

read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

project = 'dhyara'
copyright = '2021, Sunanda Bose (a.k.a. Sunanda Bose)'
author = 'Sunanda Bose (a.k.a. Sunanda Bose)'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    "exhale",
    "sphinx.ext.todo",
    "sphinx.ext.autosectionlabel", 
    "sphinx.ext.graphviz",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autodoc",
    "sphinx.ext.mathjax",
    "sphinx.ext.viewcode"
]
breathe_projects = {}
breathe_default_project = "dhyara"
autosectionlabel_prefix_document = True
autosectionlabel_maxdepth = 3
todo_include_todos = True
#breathe_debug_trace_directives = True
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./api",
    "rootFileName":          "root.rst",
    "rootFileTitle":         "API Docs",
    "doxygenStripFromPath":  "../..",
    # Suggested optional arguments
    "createTreeView":        True,
    # TIP: if using the sphinx-bootstrap-theme, you need
    # "treeViewIsBootstrap": True,
    #"exhaleExecutesDoxygen": True,
    #"exhaleDoxygenStdin":    "INPUT = ../../includes"
}

# Tell sphinx what the primary language being documented is.
default_domain = 'cpp'
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'alabaster'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

if not read_the_docs_build:
    html_theme = "karma_sphinx_theme"
