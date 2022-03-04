const ctx = document.getElementById('myChart').getContext('2d');
const myChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: ['1', '2', '3', '4', '5', '6','8','9'],
        datasets: [{
            label: 'about data',
            data: [12, 19, 3, 5, 2, 3,14,15],
                backgroundColor: ['#8985f5'],
            borderColor: [
                '#8985f5',
            ],
            borderWidth: 1
        }]
    },
    options: {
        animations: {
            tension: {
              duration: 150000,
              easing: 'linear',
              from: 0.5,
              to: 0.8,
              loop: true
            }
          },
        scales: {
            y: {
                min: 0,
                max: 25
            }
        },
        suggestedMin: -10,
        suggestedMax: 200
    }
});