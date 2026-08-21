# OpenAIRAC Procedure Sources & Taxonomy Specification (v2.1.0)

## 1. The Source Taxonomy

Aviation data exists across distinct legal, technical, and structural formats. OpenAIRAC defines four first-class source taxonomy classes:

```text
+─────────────────────────────────────────────────────────────────────────────+
|                          SOURCE TAXONOMY CLASSES                            |
+─────────────────────────────────────────────────────────────────────────────+
| 1. StructuredNavDataset           | Complete machine-readable datasets       |
|                                   | (e.g. FAA CIFP ARINC 424, AIXM 5.1/4.5)  |
|                                   |                                         |
| 2. StructuredProcedurePublication | Explicit tabular coding tables published |
|                                   | by authorities in official eAIPs        |
|                                   | (e.g. SIA DATA SID/STAR/RNP, ENAIRE)    |
|                                   |                                         |
| 3. HumanReadableChart             | Graphical chart plates (PDF, PNG, JPG)  |
|                                   | (e.g. FAA d-TPP, SIA IAC / VAC / SID)    |
|                                   |                                         |
| 4. DerivedGeometry                | Reconstructed or computed geometry       |
|                                   | (e.g. FIR boundaries, WMM2025 grids)     |
+─────────────────────────────────────────────────────────────────────────────+
```

---

## 2. Inviolable Rule of Procedure Ingestion

* **`StructuredNavDataset` & `StructuredProcedurePublication`**: Legitimate sources for canonical navigation procedures because the civil aviation authority explicitly encodes path terminators (`IF`, `TF`, `CF`, `DF`), courses, distances, and constraints.
* **`HumanReadableChart`**: Never used to synthesize or guess procedure legs via OCR, machine vision, or geometric fitting.
* **No Invented Waypoints**: When a structured table references an unlocated fix, it remains unresolved; OpenAIRAC never invents coordinates.
