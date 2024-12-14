# Project Report: Correlation Analysis Between Math/Science and Reading/Writing Scores

## Introduction

### Foreword

My proposal was to classify hypoglycemic events using a Diabetes dataset. Unfortunately, after many iterations and hours, it just didn't align with the project. I moved `dnn.cpp` to this directory, and the comment at the top explains my journey through that dataset... 

Last semester I took Machine Intelligence with Dr. Crowley, we did similar analysis but in Python, which I have much more experience and comfort in.

### Dataset Overview

The dataset utilized in this project comprises comprehensive academic records of **9,786 students**. Each student entry includes the following attributes:

- **Roll Number**: Unique identifier for each student.
- **Gender**: Male or Female.
- **Race/Ethnicity**: Classification based on racial and ethnic backgrounds.
- **Parental Level of Education**: Highest education level attained by parents.
- **Lunch**: Indicates whether the student receives free/reduced lunch (1) or not (0).
- **Test Preparation Course**: Indicates whether the student completed a test preparation course (1) or not (0).
- **Math Score** (0-100)
- **Science Score** (0-100)
- **Reading Score** (0-100)
- **Writing Score** (0-100)
- **Total Score**: Aggregated score from all subjects. (0-400)
- **Grade**: Overall grade assigned to the student. (A-F)

The csv zip can be downloaded from: https://www.kaggle.com/datasets/nadeemajeedch/students-performance-10000-clean-data-eda

NOTE: rename `student_performace.csv` to `data.csv`

### Motivation

Understanding the relationships between different academic disciplines is crucial for educators and policymakers aiming to develop effective teaching strategies and support systems. Specifically, examining the correlation between quantitative subjects (**Math** and **Science**) and language-based subjects (**Reading** and **Writing**) can provide insights into whether proficiency in one domain influences performance in another. My analysis seeks to determine if these correlations exist, thereby informing holistic educational approaches that address multiple facets of student abilities.

### Hypothesis

**Primary Hypothesis:** There exists a significant correlation between students’ Math and Science scores and their Reading and Writing scores, suggesting that proficiency in quantitative subjects is associated with performance in language-based subjects.

## Data Analysis Techniques

To evaluate the hypothesis, the following analytical methods were employed:

1. **Data Parsing and Cleaning:** The dataset was meticulously processed to handle missing or inconsistent entries, ensuring the reliability of subsequent analyses.
2. **Descriptive Statistics:** Computed basic statistical measures (mean, median, standard deviation) for Math, Science, Reading, and Writing scores to understand their distributions.
3. **Pearson Correlation Coefficient:** Calculated to quantify the strength and direction of the linear relationships between:
   - Math and Science scores
   - Reading and Writing scores
4. **Data Visualization:** Generated scatter plots with regression lines to visually assess the correlations between the selected pairs of scores.
5. **Statistical Significance Testing:** Determined the p-values associated with each correlation coefficient to assess their statistical significance.

I used this struct to store student information:

```c++
struct Student
{
    std::string roll_no;
    std::string gender;
    std::string race_ethnicity;
    std::string parental_level_of_education;
    int lunch;                   // 1: free/reduced, 0: not
    int test_preparation_course; // 1: completed, 0: not
    int math_score;
    int science_score;
    int reading_score;
    int writing_score;
    int total_score;
    char grade;
};
```

These techniques facilitate a examination of the relationships between quantitative and language-based academic performances.

## Interpretation

- **Math and Science (r = -0.0354):** Indicates a negligible negative correlation, suggesting that performance in Math and Science is largely independent of each other. This implies that proficiency in one does not predict proficiency in the other.
- **Reading and Writing (r = 0.0978):** Demonstrates a weak positive correlation, indicating a slight association between Reading and Writing scores. While there is a trend where higher Reading scores are somewhat associated with higher Writing scores, the relationship is not strong.
- **Math and Total (r = 0.5780):** Reflects a moderate positive correlation, suggesting that higher Math scores are associated with higher Total scores. This indicates that Math performance is a significant contributor to overall academic achievement.
- **Science and Total (r = 0.4708):** Shows a moderate positive correlation, indicating that better Science scores are linked to higher Total scores. Science performance also plays a substantial role in overall academic success.
- **Reading and Total (r = 0.5372):** Reveals a moderate positive correlation, suggesting that higher Reading scores are associated with higher Total scores. Reading proficiency significantly contributes to overall academic performance.

## Statistical Significance

Both correlation coefficients were tested for statistical significance. Given the large sample size (**n = 9,786**), even small correlation coefficients can achieve statistical significance. However, the practical significance of these correlations remains minimal due to their low magnitude...

## Conclusion

My analysis of the dataset provides the following insights regarding the hypothesis:

- **Math and Science Correlation:** The negligible negative correlation (-0.0354) between Math and Science scores suggests that there is virtually no relationship between a student’s performance in these two quantitative subjects. This indicates that proficiency in Math does not predict proficiency in Science and vice versa.
- **Reading and Writing Correlation:** The weak positive correlation (0.0978) between Reading and Writing scores indicates a slight association, where higher Reading scores are marginally related to higher Writing scores. However, the strength of this relationship is minimal, suggesting that other factors may play a more significant role in determining performance in these areas.

### Limitations

- **Scope of Analysis:** The current analysis focused solely on the pairwise correlations between Math and Science, and Reading and Writing. It did not explore cross-correlations between quantitative and language-based subjects (Math and Reading), which could provide a more comprehensive understanding of intersubject relationships.