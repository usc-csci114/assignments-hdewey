#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <random>

#include <matplot/matplot.h>
#include <dlib/dnn.h>
#include <dlib/matrix.h>

using namespace matplot;

/*

NOTE: 
I'm uploading this as an artifact. I spent a lot of time on it, but didn't like it for my final submission. 
I started with a CNN, however, it was taking the easiest option of always guessing that there was no hypoglycemic event.
This was because of a class imbalace, (89% non-hypo, 11% hypo) that interfered with learning. 
I then oversampled hypo events, which allowed a DNN to reach:

Confusion Matrix:
TP: 108 FP: 1199
FN: 114 TN: 1436
Accuracy: 0.540427
Precision (hypo=1): 0.082632
Recall (hypo=1): 0.486486
F1-score (hypo=1): 0.141269

Funny enough, this was more promising than my CNN, which was 92% accurate, but I still decided to scrap this route.

*/

// net architecture
using net_type = dlib::loss_multiclass_log<
    dlib::fc<2,
        dlib::relu<dlib::fc<10,
            dlib::input<dlib::matrix<double>>>>>>;

struct DiabetesRecord
{
    int patient_id;
    std::chrono::system_clock::time_point timestamp;
    int code;
    double value;
};

struct AggregatedData
{
    int bg_count = 0;
    double bg_sum = 0.0;
    double bg_min = 1e9;
    double bg_max = -1e9;
    double insulin_regular = 0.0;
    double insulin_nph = 0.0;
    double insulin_ultralente = 0.0;
    int meal_typical = 0;
    int meal_more = 0;
    int meal_less = 0;
    int exercise_typical = 0;
    int exercise_more = 0;
    int exercise_less = 0;
    bool hypo = false;
};

// func to parse date and time strings into time_point
static std::chrono::system_clock::time_point parseDateTime(const std::string &date, const std::string &time)
{
    std::tm tm_ = {};
    int MM, DD, YYYY;
    char dash1, dash2;
    {
        std::istringstream iss(date);
        iss >> MM >> dash1 >> DD >> dash2 >> YYYY;
        tm_.tm_year = YYYY - 1900;
        tm_.tm_mon = MM - 1;
        tm_.tm_mday = DD;
    }
    
    {
        int HH, mm;
        char colon;
        std::istringstream iss(time);
        iss >> HH >> colon >> mm;
        tm_.tm_hour = HH;
        tm_.tm_min = mm;
    }
    tm_.tm_sec = 0;
    tm_.tm_isdst = -1;
    std::time_t t = std::mktime(&tm_);
    return std::chrono::system_clock::from_time_t(t);
}

// func to bucket time into hours
static std::time_t toHourBucket(std::chrono::system_clock::time_point tp)
{
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm *tm_ = std::localtime(&tt);
    tm_->tm_min = 0;
    tm_->tm_sec = 0;
    return std::mktime(tm_);
}

int main()
{
    std::string dataDir = "Diabetes-Data";
    std::vector<DiabetesRecord> allRecords;

    // load data
    for (const auto &entry : std::filesystem::directory_iterator(dataDir))
    {
        if (entry.is_regular_file())
        {
            std::string filename = entry.path().filename().string();
            if (filename.rfind("data-", 0) == 0)
            { // Files starting with "data-"
                int patient_id = std::stoi(filename.substr(5));
                std::ifstream file(entry.path());
                std::string line;
                while (std::getline(file, line))
                {
                    if (line.empty())
                        continue;
                    std::istringstream iss(line);
                    std::string date, time;
                    int code;
                    double val;
                    if (std::getline(iss, date, '\t') &&
                        std::getline(iss, time, '\t') &&
                        (iss >> code) &&
                        (iss >> val))
                    {
                        auto timestamp = parseDateTime(date, time);
                        allRecords.push_back({patient_id, timestamp, code, val});
                    }
                }
            }
        }
    }

    // sort records chronologically
    std::sort(allRecords.begin(), allRecords.end(), [](const DiabetesRecord &a, const DiabetesRecord &b)
        { return a.timestamp < b.timestamp; });

    // aggregate by hour
    std::map<std::pair<int, std::time_t>, AggregatedData> hourlyData;
    for (auto &rec : allRecords)
    {
        auto bucket = toHourBucket(rec.timestamp);
        auto key = std::make_pair(rec.patient_id, bucket);

        if (rec.code >= 48 && rec.code <= 64)
        { // Blood Glucose Codes
            double bg = rec.value;
            auto &agg = hourlyData[key];
            agg.bg_count++;
            agg.bg_sum += bg;
            if (bg < agg.bg_min)
                agg.bg_min = bg;
            if (bg > agg.bg_max)
                agg.bg_max = bg;
            if (bg < 70.0)
                agg.hypo = true;
        }
        else if (rec.code == 33)
        { // Insulin Regular
            hourlyData[key].insulin_regular += rec.value;
        }
        else if (rec.code == 34)
        { // Insulin NPH
            hourlyData[key].insulin_nph += rec.value;
        }
        else if (rec.code == 35)
        { // Insulin Ultralente
            hourlyData[key].insulin_ultralente += rec.value;
        }
        else if (rec.code == 66)
        { // Meal Typical
            hourlyData[key].meal_typical += 1;
        }
        else if (rec.code == 67)
        { // Meal More
            hourlyData[key].meal_more += 1;
        }
        else if (rec.code == 68)
        { // Meal Less
            hourlyData[key].meal_less += 1;
        }
        else if (rec.code == 69)
        { // Exercise Typical
            hourlyData[key].exercise_typical += 1;
        }
        else if (rec.code == 70)
        { // Exercise More
            hourlyData[key].exercise_more += 1;
        }
        else if (rec.code == 71)
        { // Exercise Less
            hourlyData[key].exercise_less += 1;
        }
    }

    // features and labels
    std::map<int, std::vector<std::time_t>> patientHours;
    for (auto &kv : hourlyData)
        patientHours[kv.first.first].push_back(kv.first.second);
    for (auto &ph : patientHours)
        std::sort(ph.second.begin(), ph.second.end());

    std::vector<std::vector<double>> X;
    std::vector<unsigned long> y;

    for (auto &ph : patientHours)
    {
        int pid = ph.first;
        auto &hours = ph.second;
        for (size_t i = 0; i + 1 < hours.size(); i++)
        {
            auto currKey = std::make_pair(pid, hours[i]);
            auto nextKey = std::make_pair(pid, hours[i + 1]);
            auto &currAgg = hourlyData[currKey];
            auto &nextAgg = hourlyData[nextKey];

            double avg_bg = (currAgg.bg_count > 0) ? (currAgg.bg_sum / currAgg.bg_count) : 150.0;
            double insulin_total = currAgg.insulin_regular + currAgg.insulin_nph + currAgg.insulin_ultralente;
            double meal_count = currAgg.meal_typical + currAgg.meal_more + currAgg.meal_less;
            double exercise_count = currAgg.exercise_typical + currAgg.exercise_more + currAgg.exercise_less;

            double f_bg_min = (currAgg.bg_min == 1e9) ? 150.0 : currAgg.bg_min;
            double f_bg_max = (currAgg.bg_max == -1e9) ? 150.0 : currAgg.bg_max;

            std::vector<double> features = {
                f_bg_min,
                f_bg_max,
                avg_bg,
                insulin_total,
                meal_count,
                exercise_count};

            X.push_back(features);
            y.push_back(nextAgg.hypo ? 1UL : 0UL);
        }
    }

    // print basic stats
    std::cout << "Total samples: " << X.size() << "\n";
    int total_hypo = static_cast<int>(std::count(y.begin(), y.end(), 1UL));
    int total_nonhypo = static_cast<int>(X.size()) - total_hypo;
    std::cout << "Total hypo (label=1): " << total_hypo << "\n";
    std::cout << "Total non-hypo (label=0): " << total_nonhypo << "\n";

    // test/train split
    int train_size = static_cast<int>(X.size() * 0.8);
    std::vector<std::vector<double>> X_train(X.begin(), X.begin() + train_size);
    std::vector<unsigned long> y_train(y.begin(), y.begin() + train_size);

    std::vector<std::vector<double>> X_test(X.begin() + train_size, X.end());
    std::vector<unsigned long> y_test(y.begin() + train_size, y.end());

    std::cout << "Train size: " << X_train.size() << "\n";
    int train_hypo = static_cast<int>(std::count(y_train.begin(), y_train.end(), 1UL));
    int train_nonhypo = static_cast<int>(X_train.size()) - train_hypo;
    std::cout << "Train hypo: " << train_hypo << "\n";
    std::cout << "Train non-hypo: " << train_nonhypo << "\n";

    std::cout << "Test size: " << X_test.size() << "\n";
    int test_hypo = static_cast<int>(std::count(y_test.begin(), y_test.end(), 1UL));
    int test_nonhypo = static_cast<int>(X_test.size()) - test_hypo;
    std::cout << "Test hypo: " << test_hypo << "\n";
    std::cout << "Test non-hypo: " << test_nonhypo << "\n";

    // feature normilization
    size_t feat_dim = X_train[0].size();
    std::vector<double> mean_vals(feat_dim, 0.0), std_vals(feat_dim, 0.0);

    // mean
    for (auto &sample : X_train)
    {
        for (size_t f = 0; f < feat_dim; f++)
            mean_vals[f] += sample[f];
    }
    for (size_t f = 0; f < feat_dim; f++)
        mean_vals[f] /= X_train.size();

    // standard deviation
    for (auto &sample : X_train)
    {
        for (size_t f = 0; f < feat_dim; f++)
        {
            double diff = sample[f] - mean_vals[f];
            std_vals[f] += diff * diff;
        }
    }
    for (size_t f = 0; f < feat_dim; f++)
    {
        std_vals[f] = std::sqrt(std_vals[f] / X_train.size());
        if (std_vals[f] < 1e-9)
            std_vals[f] = 1.0; // do not divide by zero
    }

    // normalize func
    auto normalize_fn = [&](std::vector<double> &feats)
    {
        for (size_t f = 0; f < feats.size(); f++)
        {
            feats[f] = (feats[f] - mean_vals[f]) / std_vals[f];
        }
    };

    // normalization
    for (auto &sample : X_train)
        normalize_fn(sample);
    for (auto &sample : X_test)
        normalize_fn(sample);

    // overcome imbalance by oversampling hypo class:
    if (train_hypo < train_nonhypo)
    {
        std::vector<std::vector<double>> minority_samples;
        for (size_t i = 0; i < X_train.size(); i++)
            if (y_train[i] == 1UL)
                minority_samples.push_back(X_train[i]);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, static_cast<int>(minority_samples.size()) - 1);

        while (train_hypo < train_nonhypo)
        {
            int idx = dist(gen);
            X_train.push_back(minority_samples[idx]);
            y_train.push_back(1UL);
            train_hypo++;
        }
        std::cout << "After oversampling:\n";
        std::cout << "Train size: " << X_train.size() << "\n";
        std::cout << "Train hypo: " << train_hypo << "\n";
        std::cout << "Train non-hypo: " << (static_cast<int>(X_train.size()) - train_hypo) << "\n";
    }

    // dlib samplessss
    std::vector<dlib::matrix<double>> dlib_train_samples(X_train.size());
    std::vector<dlib::matrix<double>> dlib_test_samples(X_test.size());

    for (size_t i = 0; i < X_train.size(); i++)
    {
        dlib::matrix<double> samp(feat_dim, 1);
        for (size_t f = 0; f < feat_dim; f++)
            samp(f) = X_train[i][f];
        dlib_train_samples[i] = samp;
    }

    std::vector<unsigned long> train_labels_final = y_train;

    for (size_t i = 0; i < X_test.size(); i++)
    {
        dlib::matrix<double> samp(feat_dim, 1);
        for (size_t f = 0; f < feat_dim; f++)
            samp(f) = X_test[i][f];
        dlib_test_samples[i] = samp;
    }

    std::vector<unsigned long> test_labels_final = y_test;

    // train dnn
    net_type net;

    dlib::dnn_trainer<net_type> trainer(net);
    trainer.set_learning_rate(0.01);
    trainer.set_min_learning_rate(0.0001);
    trainer.set_mini_batch_size(64);
    trainer.set_iterations_without_progress_threshold(2000);
    trainer.set_max_num_epochs(20);
    trainer.be_verbose();

    // shuffle
    dlib::randomize_samples(dlib_train_samples, train_labels_final);

    // train
    trainer.train(dlib_train_samples, train_labels_final);

    // eval
    int tp = 0, fp = 0, fn = 0, tn = 0;
    for (size_t i = 0; i < dlib_test_samples.size(); i++)
    {
        unsigned long pred_label = net(dlib_test_samples[i]);
        unsigned long true_label = test_labels_final[i];

        if (pred_label == 1 && true_label == 1)
            tp++;
        else if (pred_label == 1 && true_label == 0)
            fp++;
        else if (pred_label == 0 && true_label == 1)
            fn++;
        else if (pred_label == 0 && true_label == 0)
            tn++;
    }

    double accuracy = static_cast<double>(tp + tn) / (tp + tn + fp + fn);
    double precision = (tp + fp) > 0 ? static_cast<double>(tp) / (tp + fp) : 0.0;
    double recall = (tp + fn) > 0 ? static_cast<double>(tp) / (tp + fn) : 0.0;
    double f1 = (precision + recall > 0) ? 2 * (precision * recall) / (precision + recall) : 0.0;

    std::cout << "Confusion Matrix:\n";
    std::cout << "TP: " << tp << " FP: " << fp << "\n";
    std::cout << "FN: " << fn << " TN: " << tn << "\n";
    std::cout << "Accuracy: " << accuracy << "\n";
    std::cout << "Precision (hypo=1): " << precision << "\n";
    std::cout << "Recall (hypo=1): " << recall << "\n";
    std::cout << "F1-score (hypo=1): " << f1 << "\n";

    // matplot plots

    // Distribution of Blood Glucose Levels (Histograms)
    std::vector<double> bg_min_values_plot;
    std::vector<double> bg_max_values_plot;
    std::vector<double> avg_bg_values_plot;

    for (auto &sample : X_train)
    {
        bg_min_values_plot.push_back(sample[0]);
        bg_max_values_plot.push_back(sample[1]);
        avg_bg_values_plot.push_back(sample[2]);
    }

    figure();

    auto plot_ax1 = subplot(3, 1, 1);
    hist(plot_ax1, bg_min_values_plot, 50);
    xlabel("BG Min");
    ylabel("Frequency");

    auto plot_ax2 = subplot(3, 1, 2);
    hist(plot_ax2, bg_max_values_plot, 50);
    xlabel("BG Max");
    ylabel("Frequency");

    auto plot_ax3 = subplot(3, 1, 3);
    hist(plot_ax3, avg_bg_values_plot, 50);
    xlabel("Average BG");
    ylabel("Frequency");

    save("blood_glucose_distribution.png");

    // Insulin Dosage Types Distribution (Histogram)
    std::vector<double> insulin_total_values_plot;
    std::vector<double> insulin_non_hypo_plot, insulin_hypo_plot;

    for (size_t i = 0; i < X_train.size(); i++)
    {
        insulin_total_values_plot.push_back(X_train[i][3]); // insulin_total is index 3
        if (y_train[i] == 1UL)
            insulin_hypo_plot.push_back(X_train[i][3]);
        else
            insulin_non_hypo_plot.push_back(X_train[i][3]);
    }

    figure();
    auto ax_insulin = subplot(1, 1, 1);
    hist(ax_insulin, insulin_non_hypo_plot, 50);
    auto ih1 = hist(ax_insulin, insulin_non_hypo_plot, 50);
    ih1->bin_width(0.5);
    ih1->face_alpha(0.5);

    auto ih2 = hist(ax_insulin, insulin_hypo_plot, 50);
    ih2->bin_width(0.5);
    ih2->face_alpha(0.5);

    title("Insulin Dosage Distribution");
    xlabel("Insulin Total");
    ylabel("Frequency");

    // legend labels
    std::vector<std::string> insulin_labels = {"Non-Hypo", "Hypo"};

    // legend
    ::matplot::legend(insulin_labels);

    save("insulin_dosage_distribution.png");

    // Meal Counts vs. Hypoglycemic Events (Scatter Plot)
    std::vector<double> meal_counts_plot;
    std::vector<double> meal_labels_double_plot;

    for (size_t i = 0; i < X_train.size(); i++)
    {
        meal_counts_plot.push_back(X_train[i][4]); // meal_count is index 4
        meal_labels_double_plot.push_back(static_cast<double>(y_train[i]));
    }

    figure();
    scatter(meal_counts_plot, meal_labels_double_plot, 10.0);
    title("Meal Counts vs. Hypoglycemic Events");
    xlabel("Meal Count");
    ylabel("Hypo Event (0 or 1)");
    save("meal_counts_vs_hypo.png");

    // Exercise Counts vs. Hypoglycemic Events (Box Plots)
    std::vector<double> exercise_non_hypo_plot, exercise_hypo_plot;
    for (size_t i = 0; i < y_train.size(); i++)
    {
        if (y_train[i] == 1UL)
            exercise_hypo_plot.push_back(X_train[i][5]); // exercise_count is index 5
        else
            exercise_non_hypo_plot.push_back(X_train[i][5]);
    }

    std::vector<std::vector<double>> exercise_data_plot = {exercise_non_hypo_plot, exercise_hypo_plot};

    figure();
    boxplot(exercise_data_plot);
    title("Exercise Counts vs. Hypoglycemic Events");
    ylabel("Exercise Count");
    xticks({1, 2});
    xticklabels({"Non-Hypo", "Hypo"});
    save("exercise_counts_boxplot.png");

    // Training Loss Over Epochs (Line Graph)
    std::vector<double> epoch_loss_plot;

    // Re-init the trainer for tracking loss
    dlib::dnn_trainer<net_type> trainer_tracking(net);
    trainer_tracking.set_learning_rate(0.01);
    trainer_tracking.set_min_learning_rate(0.0001);
    trainer_tracking.set_mini_batch_size(64);
    trainer_tracking.set_iterations_without_progress_threshold(2000);
    trainer_tracking.set_max_num_epochs(20);
    trainer_tracking.be_verbose();

    // Shuffle data before training
    dlib::randomize_samples(dlib_train_samples, train_labels_final);

    for (int epoch = 0; epoch < 20; epoch++)
    {
        trainer_tracking.train_one_step(dlib_train_samples, train_labels_final);
        double current_loss = trainer_tracking.get_average_loss();
        epoch_loss_plot.push_back(current_loss);
        std::cout << "Epoch " << epoch + 1 << " average loss: " << current_loss << "\n";
    }

    figure();
    plot(epoch_loss_plot);
    title("Training Loss Over Epochs");
    xlabel("Epoch");
    ylabel("Average Loss");
    save("training_loss.png");

    // final eval
    int tp_final = 0, fp_final = 0, fn_final = 0, tn_final = 0;
    for (size_t i = 0; i < dlib_test_samples.size(); i++)
    {
        unsigned long pred_label = net(dlib_test_samples[i]);
        unsigned long true_label = test_labels_final[i];

        if (pred_label == 1 && true_label == 1)
            tp_final++;
        else if (pred_label == 1 && true_label == 0)
            fp_final++;
        else if (pred_label == 0 && true_label == 1)
            fn_final++;
        else if (pred_label == 0 && true_label == 0)
            tn_final++;
    }

    double accuracy_final = static_cast<double>(tp_final + tn_final) / (tp_final + tn_final + fp_final + fn_final);
    double precision_final = (tp_final + fp_final) > 0 ? static_cast<double>(tp_final) / (tp_final + fp_final) : 0.0;
    double recall_final = (tp_final + fn_final) > 0 ? static_cast<double>(tp_final) / (tp_final + fn_final) : 0.0;
    double f1_final = (precision_final + recall_final > 0) ? 2 * (precision_final * recall_final) / (precision_final + recall_final) : 0.0;

    std::cout << "Confusion Matrix:\n";
    std::cout << "TP: " << tp_final << " FP: " << fp_final << "\n";
    std::cout << "FN: " << fn_final << " TN: " << tn_final << "\n";
    std::cout << "Accuracy: " << accuracy_final << "\n";
    std::cout << "Precision (hypo=1): " << precision_final << "\n";
    std::cout << "Recall (hypo=1): " << recall_final << "\n";
    std::cout << "F1-score (hypo=1): " << f1_final << "\n";

    return 0;
}