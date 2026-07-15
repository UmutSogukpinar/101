using fan_league.Entities;
using Microsoft.EntityFrameworkCore;

namespace fan_league.Data.Seeds;

public static class NbaDataSeeder
{
    private const int ShaiNbaId = 1628983;

    public static async Task SeedAsync(
        AppDbContext dbContext,
        CancellationToken cancellationToken = default)
    {
        NbaPlayer? player = await dbContext.NbaPlayers
            .FirstOrDefaultAsync(
                nbaPlayer => nbaPlayer.NbaId == ShaiNbaId,
                cancellationToken);

        if (player is null)
        {
            player = new NbaPlayer
            {
                NbaId = ShaiNbaId,
                FirstName = "Shai",
                LastName = "Gilgeous-Alexander",
                Team = "Oklahoma City Thunder",
                Position = "Guard",
                JerseyNumber = 2,
                HeightCm = 198,
                WeightKg = 88,
                IsActive = true
            };

            await dbContext.NbaPlayers.AddAsync(player, cancellationToken);
            await dbContext.SaveChangesAsync(cancellationToken);
        }

        bool seasonStatsExist = await dbContext.NbaPlayerSeasonStats.AnyAsync(
            seasonStat => seasonStat.NbaPlayerId == player.Id
                && seasonStat.Season == 2025,
            cancellationToken);

        if (seasonStatsExist)
        {
            return;
        }

        NbaPlayerSeasonStat stats = new()
        {
            NbaPlayerId = player.Id,
            NbaPlayer = player,
            Season = 2025,
            GamesPlayed = 76,
            GamesStarted = 76,
            MinutesPerGame = 34.2m,
            PointsPerGame = 32.7m,
            ReboundsPerGame = 5.0m,
            AssistsPerGame = 6.4m,
            StealsPerGame = 1.7m,
            BlocksPerGame = 1.0m,
            TurnoversPerGame = 2.4m,
            FieldGoalPercentage = 51.9m,
            ThreePointPercentage = 37.5m,
            FreeThrowPercentage = 89.8m
        };

        await dbContext.NbaPlayerSeasonStats.AddAsync(stats, cancellationToken);
        await dbContext.SaveChangesAsync(cancellationToken);
    }
}
