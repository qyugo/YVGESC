---
title: Component Datasheets
parent: Hardware Design
nav_order: 1
---

## Components and Datasheets
--------------------

Relevant components for the PCB. Click a card to open its datasheet.

<div class="parts">
{%- for p in site.data.components -%}
<a class="part" href="{{ p.datasheet | relative_url }}" target="_blank" rel="noopener">
<img src="{{ p.image | relative_url }}" alt="{{ p.name }}">
<div class="part-body">
<p class="part-name">{{ p.name }}</p>
<p class="part-meta">{{ p.role }}</p>
</div>
</a>
{%- endfor -%}
</div>
