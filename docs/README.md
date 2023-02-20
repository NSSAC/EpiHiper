# Writing documentation

The documentation for EpiHiper is written in [reStructuredText](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html#lists-and-quote-like-blocks)


### Adding a mathematical formula to the content:

#### Block equation

```
.. math::

  (a + b)^2 = a^2 + 2ab + b^2

```

For more information please see: [math directive](https://www.sphinx-doc.org/en/master/usage/restructuredtext/directives.html#directive-math)

#### Inline equation

```
  Since Pythagoras, we know that :math:`a^2 + b^2 = c^2`.

```

For more information please see: [math role](https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html#role-math)